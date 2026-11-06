# Libasm — Assembly yourself!

> Subject version 6.0. Condensed from `en.subject.pdf`: foreword, introduction,
> AI-usage chapter, submission boilerplate and the bonus part removed. Only the
> global rules and the mandatory per-function requirements are kept.

---

## Global rules

### Common instructions

- Your functions must not quit unexpectedly (segfault, bus error, double free…)
  except on undefined behaviour. If that happens the project is non-functional
  and graded **0**.
- The Makefile must contain at least `$(NAME)`, `all`, `clean`, `fclean`, `re`.
  It must recompile/relink only the necessary files.
- Test programs are encouraged but not submitted or graded.
- Only work pushed to the assigned Git repository is graded.

### Assembly rules

- You must write **64-bit** assembly. Beware of the **calling convention**.
  > The convention is the rules of how registers are used and for what. Ours is
  > the SysV AMD64 ABI. We comply: `bits 64`, args from `rdi/rsi/rdx`, return in
  > `rax`, `rbx` saved in `ft_strdup`, stack 16-byte aligned at each `call`.

- **No inline ASM** — you must write `.s` files.
  > Inline asm sits inside a C file and lets the compiler write the function for
  > you. We comply: six standalone `.s` files, no `__asm__` in the C.

- You must compile your assembly with **nasm**.
  > We comply: `nasm -f elf64`, where `-f` sets the output format.

- You must use **Intel syntax**, not AT&T syntax.
  > Intel puts the destination first with bare names (`mov rax, rdi`). 
  > nasm only accepts Intel.

- It is **forbidden** to use the compilation flag `-no-pie`.
  > `-no-pie` disables position-independent code. We comply: not used, so calls
  > to shared libraries need `wrt ..plt` — it routes them through the PLT, which
  > the dynamic linker fills in at runtime, since libc's load address is not
  > known until then. Only `malloc` and `__errno_location` need it; syscalls do
  > not, as `syscall` takes a number in `rax`, not an address.

---

## Mandatory part

- The library must be called `libasm.a`.
- You must submit a `main` function that tests your functions and compiles
  against your library, to demonstrate that it works.
- You must check for errors during syscalls and handle them properly when
  needed.
- Your code must set `errno` properly. For that you are allowed to call the
  extern `___error` or `__errno_location`.

### Functions to rewrite in assembly

| Function | Reference | Extra requirement |
|---|---|---|
| `ft_strlen` | `man 3 strlen` | — |
| `ft_strcpy` | `man 3 strcpy` | — |
| `ft_strcmp` | `man 3 strcmp` | — |
| `ft_write` | `man 2 write` | check the syscall, set `errno` on failure |
| `ft_read` | `man 2 read` | check the syscall, set `errno` on failure |
| `ft_strdup` | `man 3 strdup` | you may call `malloc` |
| `ft_calloc` | `man 3 calloc` | you may call `malloc`; you must **detect and handle the `size_t` overflow in `nmemb * size` yourself**, instead of relying on malloc to catch it |
