# libasm notes (for absolute beginners)

## constraints

- 64-bit assembly only
- `.s` files (no inline ASM)
- assemble with `nasm`
- Intel syntax (not AT&T)
- follow the calling convention (System V AMD64 ABI on Linux x86_64)

---

## 1) registers

Registers are tiny, super-fast storage slots inside the CPU. Think of them as variables that live in the CPU itself.

In 64-bit mode the general-purpose registers are:

| Register | Caller/Callee-saved | Common use |
|----------|-------------------|------------|
| `RAX` | caller | return value, syscall number, scratch |
| `RBX` | **callee** | general purpose (save/restore if used) |
| `RCX` | caller | scratch, 4th syscall arg (as `R10`) |
| `RDX` | caller | 3rd argument, scratch |
| `RSI` | caller | 2nd argument |
| `RDI` | caller | 1st argument |
| `RBP` | **callee** | frame pointer (avoid for now) |
| `RSP` | **callee** | stack pointer — managed by push/pop/call/ret |
| `R8`–`R9` | caller | 5th and 6th arguments |
| `R10`–`R11` | caller | scratch |
| `R12`–`R15` | **callee** | general purpose (save/restore if used) |

**caller-saved** — you can use freely; no obligation to restore before `ret`

**callee-saved** — if your function modifies these, you must `push` them at the start and `pop` them before `ret`, so the caller finds them unchanged

### register aliasing

One physical register can be accessed at different widths using different names.
These are called **aliases** — they all refer to overlapping parts of the same register.

| 64-bit | 32-bit | 16-bit | 8-bit |
|--------|--------|--------|-------|
| `RAX` | `EAX` | `AX` | `AL` |
| `RBX` | `EBX` | `BX` | `BL` |
| `RCX` | `ECX` | `CX` | `CL` |
| `RDX` | `EDX` | `DX` | `DL` |

The 8-bit aliases (`AL`, `BL`, `CL`, `DL`) are commonly used when working with
single characters, since a `char` is 1 byte. Intel syntax requires the source and
destination sizes to match, so you can't `mov rax, byte [rdi]` — you must use `AL`.

**the upper bits problem**

Writing to `AL` only touches the lowest byte of `RAX` — the upper 56 bits are left
untouched and may contain garbage from earlier operations. Before doing arithmetic
on the full register you must clear them. Two ways:

```nasm
; option 1: load into alias, zero-extend after
mov al, byte [rdi]
movzx eax, al         ; zero-extend AL into EAX (also clears upper 32 bits of RAX)

; option 2: zero-extend on load (preferred — cleaner)
movzx eax, byte [rdi] ; load 1 byte, zero-extend directly into EAX
```

`movzx` (move with zero-extension) copies a smaller value into a larger register,
filling the upper bits with zeros. Writing to a 32-bit register (`EAX`) also
automatically zero-extends into the full 64-bit register — an x86-64 rule.

**practical example — ft_strcpy**

`CL` (low byte of `RCX`) is used to ferry one character at a time:

```nasm
mov cl, byte [rsi]    ; load 1 byte from src into CL (low byte of RCX)
mov byte [rdi], cl    ; write that byte to dest
```

**practical example — ft_strcmp**

Using `movzx` on load avoids the cleanup step entirely:

```nasm
movzx eax, byte [rdi] ; load s1 byte, already zero-extended
movzx ecx, byte [rsi] ; load s2 byte, already zero-extended
sub eax, ecx          ; safe subtraction — no garbage in upper bits
```

When picking a register for byte aliasing, prefer `CL`/`DL` (caller-saved) over `BL`
(callee-saved) to avoid extra push/pop bookkeeping. See the register table above.

---

## 2) system calls

A **syscall** is how your program asks the Linux kernel to do something (write to the terminal, exit, open a file, etc.).

Each syscall has a number. You put that number in `RAX`, fill in the arguments, then call `syscall`.

### syscall register roles

| Register | Role |
|----------|------|
| `RAX` | syscall number — tells the kernel *which* operation |
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


### example: hello world + exit

```nasm
bits 64
global _start

extern __errno_location

section .data
    msg db "Hello, World!", 10
    msg_len equ $ - msg

section .text
_start:
	mov rax, 1	; syscall #1 = write()
	mov rdi, 1	; argument 1: file descriptor (stdout)
	lea rsi, [msg]	; argument 2: pointer to the string
	mov rdx, msg_len	; argument 3: length of the string
	syscall

    cmp rax, 0
    jl .error

    mov rax, 60	; syscall #60 = exit()
    mov rdi, 0	; exit 0 = success
    syscall

.error:
	mov rax, 60 
	mov rdi, 1	; exit 1 = error
	syscall
```

---

## 3) memory access

Registers live in the CPU. Memory (RAM) is outside it.

In Intel syntax, brackets mean "read from this address":

```nasm
mov rax, [rdi]    ; load 8 bytes from the address stored in RDI
mov [rdi], rax    ; store 8 bytes to the address stored in RDI
```

If `RDI` points to invalid memory, the program crashes (segfault).

---

## 4) calling convention (C ↔ ASM)

When C calls your assembly function, both sides must agree on the same rules — this agreement is called the **calling convention**.

On Linux x86_64 (System V AMD64 ABI):

- Arguments arrive in: `RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9`
- Return value goes in: `RAX`
- If you modify `RBX`, `RBP`, or `R12`–`R15`, restore them before `ret`

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

## 5) error handling

### in `_start` (standalone program)

There is no caller, so `errno` doesn't matter. Just exit with a non-zero code to signal failure (see hello world example above).

### in library functions (C ↔ ASM)

When a syscall fails, the kernel returns a negative value in `RAX` (e.g. `-9` for `EBADF`). Your function must:

1. detect the error
2. negate `RAX` to get the positive errno code
3. store it in `errno` via `__errno_location`
4. return `-1` to the caller

`errno` is thread-local — you can't write to it directly. Call `__errno_location` to get a pointer to it.

```nasm
extern __errno_location

ft_write:
    mov rax, 1            ; sys_write
    syscall

    cmp rax, 0            ; negative = error
    jl .error
    ret                   ; success: rax = bytes written

.error:
    neg rax               ; positive errno code
    push rax              ; save it — rax will be clobbered by the call
    call __errno_location ; rax = pointer to errno
    pop rcx               ; retrieve saved errno code
    mov [rax], rcx        ; *errno = error code
    mov rax, -1           ; return -1 to caller
    ret
```

**why `push`/`pop` and not a register?**
`RDI`, `RSI`, etc. are caller-saved — `__errno_location` is free to clobber them. The stack is safe because any callee must restore `RSP` before returning.

---

## 6) instructions

An **instruction** is a single operation the CPU can execute. In assembly source
code, instructions are written as **mnemonics** — human-readable names (`mov`, `inc`,
`je`) that nasm translates into binary opcodes.

### data movement

| Mnemonic | What it does |
|----------|-------------|
| `mov dst, src` | copy src into dst |
| `movzx dst, src` | copy src into dst, zero-filling upper bits |
| `lea dst, [addr]` | load the address itself (not the value at it) into dst |
| `push src` | push src onto the stack, decrement RSP |
| `pop dst` | pop top of stack into dst, increment RSP |

### arithmetic

| Mnemonic | What it does |
|----------|-------------|
| `add dst, src` | dst = dst + src |
| `sub dst, src` | dst = dst - src |
| `inc dst` | dst = dst + 1 |
| `dec dst` | dst = dst - 1 |
| `neg dst` | dst = -dst (two's complement negate) |
| `xor dst, src` | dst = dst XOR src — `xor rax, rax` is the standard way to zero a register |

### comparison and jumps

`cmp` subtracts src from dst and sets CPU flags — it does not store the result.
Jump instructions then read those flags to decide whether to jump.

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

## 7) checklist before testing a function

- [ ] symbol marked `global`
- [ ] name matches the C prototype
- [ ] arguments read from the right registers (`RDI`, `RSI`, ...)
- [ ] return value written to `RAX`
- [ ] any callee-saved registers restored before `ret`
- [ ] file uses Intel syntax and is assembled with `nasm`
- [ ] objdump

If something "almost works", 80% of the time it's a naming mismatch, wrong register, or clobbered callee-saved register.
