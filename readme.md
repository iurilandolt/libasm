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

`RAX`, `RBX`, `RCX`, `RDX`, `RSI`, `RDI`, `RBP`, `RSP`, `R8`–`R15`

One physical register can be accessed at different widths:

| Name | Width | What it accesses |
|------|-------|-----------------|
| `RAX` | 64-bit | full register |
| `EAX` | 32-bit | lower half |
| `AX` | 16-bit | lowest 16 bits |
| `AL` | 8-bit | lowest byte |

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

## 6) checklist before testing a function

- [ ] symbol marked `global`
- [ ] name matches the C prototype
- [ ] arguments read from the right registers (`RDI`, `RSI`, ...)
- [ ] return value written to `RAX`
- [ ] any callee-saved registers restored before `ret`
- [ ] file uses Intel syntax and is assembled with `nasm`

If something "almost works", 80% of the time it's a naming mismatch, wrong register, or clobbered callee-saved register.
