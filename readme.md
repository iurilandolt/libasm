# libasm notes (for absolute beginners)

## constraints

- 64-bit assembly only
- `.s` files (no inline ASM)
- assemble with `nasm`
- Intel syntax (not AT&T)
- follow the calling convention (System V AMD64 ABI on Linux x86_64)

---

## 1) instructions

An **instruction** is a single operation the CPU can execute. In assembly source
code, instructions are written as **mnemonics** — human-readable names that nasm
translates into the binary opcodes the CPU actually runs.

### data movement

| Mnemonic | What it does |
|----------|-------------|
| `mov dst, src` | copy src into dst |
| `movzx dst, src` | copy src into dst, zero-filling upper bits |
| `lea dst, [addr]` | load the address itself (not the value at it) into dst |
| `push src` | push src onto the stack, decrement RSP |
| `pop dst` | pop top of stack into dst, increment RSP |

**memory access** uses brackets — `[reg]` means "the value at this address":

```nasm
mov rax, [rdi]    ; load 8 bytes from the address stored in RDI
mov [rdi], rax    ; store 8 bytes to the address stored in RDI
```

The size of the access must be explicit when ambiguous: `byte`, `word`, `dword`, `qword`.

### arithmetic

| Mnemonic | What it does |
|----------|-------------|
| `add dst, src` | dst = dst + src |
| `sub dst, src` | dst = dst - src |
| `inc dst` | dst = dst + 1 |
| `dec dst` | dst = dst - 1 |
| `neg dst` | dst = -dst (two's complement negate) |
| `xor dst, src` | dst = dst XOR src — `xor rax, rax` is the idiomatic way to zero a register |

### comparison and jumps

`cmp` subtracts src from dst and sets CPU flags — it does not store the result.
Jump instructions read those flags to decide whether to jump.

| Mnemonic | Jumps when... |
|----------|--------------|
| `jmp label` | always |
| `je  label` | equal (zero flag set) |
| `jne label` | not equal |
| `jl  label` | less than (signed) |
| `jg  label` | greater than (signed) |
| `jae label` | above or equal (unsigned) — used for syscall error range check |

### functions and syscalls

| Mnemonic | What it does |
|----------|-------------|
| `call label` | push return address, jump to label |
| `ret` | pop return address, jump to it |
| `syscall` | hand control to the kernel using current register values |

---

## 2) registers

Registers are tiny, super-fast storage slots inside the CPU — think of them as
variables that live in the CPU itself.

| Register | Caller/Callee-saved | Common use |
|----------|-------------------|------------|
| `RAX` | caller | return value, syscall number, scratch |
| `RBX` | **callee** | general purpose (save/restore if used) |
| `RCX` | caller | scratch, 4th function argument |
| `RDX` | caller | 3rd argument, scratch |
| `RSI` | caller | 2nd argument |
| `RDI` | caller | 1st argument |
| `RBP` | **callee** | frame pointer (avoid for now) |
| `RSP` | **callee** | stack pointer — managed by push/pop/call/ret |
| `R8`–`R9` | caller | 5th and 6th arguments |
| `R10`–`R11` | caller | scratch |
| `R12`–`R15` | **callee** | general purpose (save/restore if used) |

**caller-saved** — free to use; no obligation to restore before `ret`

**callee-saved** — if your function modifies these, `push` them at the start
and `pop` before `ret` so the caller finds them unchanged

### register aliasing

One physical register can be accessed at different widths using different names —
these are called **aliases**.

| 64-bit | 32-bit | 16-bit | 8-bit |
|--------|--------|--------|-------|
| `RAX` | `EAX` | `AX` | `AL` |
| `RBX` | `EBX` | `BX` | `BL` |
| `RCX` | `ECX` | `CX` | `CL` |
| `RDX` | `EDX` | `DX` | `DL` |

The 8-bit aliases are used when working with single characters (`char` = 1 byte).
Intel syntax requires source and destination sizes to match — you can't
`mov rax, byte [rdi]`, you must use `AL`.

**the upper bits problem**

Writing to `AL` only touches the lowest byte of `RAX` — the upper 56 bits are
untouched and may contain garbage. Two ways to handle it:

```nasm
; option 1: load into alias, zero-extend after
mov al, byte [rdi]
movzx eax, al             ; zero-extend AL into EAX (also clears upper 32 bits of RAX)

; option 2: zero-extend on load (preferred)
movzx eax, byte [rdi]     ; load 1 byte, zero-extended directly into EAX
```

Writing to a 32-bit register (`EAX`) automatically zero-extends into the full
64-bit register — an x86-64 rule. Prefer `CL`/`DL` over `BL` for scratch byte
work since they are caller-saved and need no bookkeeping.

---

## 3) calling convention (C ↔ ASM)

When C calls your assembly function, both sides must agree on the same rules —
this is the **calling convention** (System V AMD64 ABI on Linux x86_64).

- Arguments arrive in: `RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9`
- Return value goes in: `RAX`
- Callee-saved registers must be restored before `ret`: `RBX`, `RBP`, `R12`–`R15`

### minimal example

```nasm
global my_answer

section .text
my_answer:
    mov rax, 42
    ret
```

Callable from C as `long my_answer(void);` — returns `42`.

---

## 4) system calls

A **syscall** is how your program asks the Linux kernel to do something.
The calling convention for syscalls mirrors the function convention but uses
`RAX` for the syscall number instead of a return value going in:

| Register | Role |
|----------|------|
| `RAX` | syscall number |
| `RDI` | argument 1 |
| `RSI` | argument 2 |
| `RDX` | argument 3 |
| `R10`, `R8`, `R9` | arguments 4–6 (rarely needed) |

After `syscall` returns, the result is in `RAX`.

### common syscall numbers (Linux x86_64)

| Number | Name | What it does |
|--------|------|--------------|
| `0` | `sys_read` | read bytes from a file descriptor |
| `1` | `sys_write` | write bytes to a file descriptor |
| `2` | `sys_open` | open a file, returns a fd |
| `3` | `sys_close` | close a file descriptor |
| `57` | `sys_fork` | fork the current process |
| `60` | `sys_exit` | terminate the process |

Full table: [https://filippo.io/linux-syscall-table/](https://filippo.io/linux-syscall-table/)

### standard file descriptors

| fd | meaning |
|----|---------|
| `0` | stdin |
| `1` | stdout |
| `2` | stderr |

### example: hello world + exit

```nasm
bits 64
global _start

section .data
    msg db "Hello, World!", 10
    msg_len equ $ - msg       ; equ $ - label = auto-calculate length

section .text
_start:
    mov rax, 1                ; sys_write
    mov rdi, 1                ; fd 1 = stdout
    lea rsi, [msg]            ; pointer to string
    mov rdx, msg_len          ; byte count
    syscall

    cmp rax, 0                ; negative return = error
    jl .error

    mov rax, 60               ; sys_exit
    mov rdi, 0                ; exit code 0 = success
    syscall

.error:
    mov rax, 60
    mov rdi, 1                ; exit code 1 = failure
    syscall
```

---

## 5) error handling

When a syscall fails, the kernel returns a negative value in `RAX`. That's all
you need to detect an error: check if `RAX < 0` after the syscall.

### in `_start` (standalone program)

No caller, so just exit with a non-zero code:

```nasm
syscall
cmp rax, 0
jl .error           ; rax is negative = something went wrong

mov rax, 60
mov rdi, 0          ; exit 0 = success
syscall

.error:
    mov rax, 60
    mov rdi, 1      ; exit 1 = failure
    syscall
```

### in library functions (C ↔ ASM)

The C caller expects two things on failure: your function returns `-1`, and the
`errno` variable is set to the error code.

The error code is just `RAX` negated — if `RAX` came back as `-9`, the errno is `9`.

`errno` is thread-local so you can't write to it directly — call `__errno_location`
to get a pointer to it. Since that call will clobber `RAX`, save the error code on
the stack first with `push`/`pop`.

```nasm
extern __errno_location

ft_write:
    mov rax, 1            ; sys_write
    syscall

    cmp rax, 0            ; negative = error
    jl .error
    ret                   ; success: rax = bytes written

.error:
    neg rax               ; flip negative to positive = errno code
    push rax              ; save it — the call below will clobber rax
    call __errno_location ; rax = pointer to errno
    pop rcx               ; get our saved errno code back
    mov [rax], rcx        ; *errno = error code
    mov rax, -1           ; return -1 to caller
    ret
```

---

## 6) checklist before testing a function

- [ ] symbol marked `global`
- [ ] name matches the C prototype
- [ ] arguments read from the right registers (`RDI`, `RSI`, ...)
- [ ] return value written to `RAX`
- [ ] callee-saved registers restored if modified
- [ ] file uses Intel syntax and is assembled with `nasm`

If something "almost works", 80% of the time it's a naming mismatch, wrong register,
or a clobbered callee-saved register.
