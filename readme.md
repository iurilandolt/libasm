# libasm notes (for absolute beginners)

This document explains the very basics you need before writing your first assembly functions.

## your learning constraints (important)

You must follow these rules:

- Write **64-bit assembly** only.
- Use **`.s` files** (no inline ASM inside C).
- Assemble with **nasm**.
- Use **Intel syntax** (not AT&T syntax).
- Follow the **calling convention** correctly.

Because you are on Linux x86_64, this usually means the **System V AMD64 ABI** calling convention.

---

## 1) what is a register?

A CPU has tiny, super-fast storage slots called **registers**.

Think of a register like a variable that lives inside the CPU.

In 64-bit mode, many general-purpose registers are 64 bits wide:

- `RAX`, `RBX`, `RCX`, `RDX`
- `RSI`, `RDI`
- `RBP`, `RSP`
- `R8` to `R15`

### same register, different sizes

One physical register can be accessed with different sizes:

- `RAX` = 64-bit
- `EAX` = low 32-bit
- `AX` = low 16-bit
- `AL` = low 8-bit

This matters when reading/writing data of different types.

---

## 1.5) are registers "reserved"?

Short answer: **not permanently**.

You can use general-purpose registers for your own temporary values.
But at important boundaries, some registers must carry specific meanings.

### context 1: inside your own function

Most registers are free to use for calculations.

### context 2: function call boundary (C <-> ASM)

By calling convention (Linux x86_64):


- `RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9` = first arguments
- `RAX` = return value
- `RBX`, `RBP`, `R12`-`R15` must be preserved by your function

### context 3: syscall boundary (Linux kernel)


For `syscall` on Linux x86_64:

- `RAX` = syscall number
- `RDI`, `RSI`, `RDX`, `R10`, `R8`, `R9` = syscall arguments
- return value comes back in `RAX`

So: registers are "free" while you work, but must match expected roles
when crossing function/syscall boundaries.

---

## 2) first instructions you should know

### `mov`

Copies data from source to destination.

- `mov rdi, 8` → put immediate value `8` in `RDI`
- `mov rsi, rdi` → copy `RDI` into `RSI`

### `add` / `sub`

- `add rax, 1` → `RAX = RAX + 1`
- `sub rax, 2` → `RAX = RAX - 2`

### `xor`

- `xor rax, rax` → set `RAX` to `0` (common fast way)

### `ret`

Return from function to the caller.

---

## 3) memory vs register

Registers are inside CPU. Memory (RAM) is outside CPU.

In Intel syntax, memory access uses brackets:

- `mov rax, [rdi]` → load 8 bytes from address in `RDI`
- `mov [rdi], rax` → store 8 bytes to address in `RDI`

If `RDI` points to invalid memory, program crashes.

---

## 4) calling convention (core concept)

When C calls your assembly function, both sides must agree on rules.
Those rules are the **calling convention**.

For Linux x86_64 (System V):

### argument registers

The first integer/pointer arguments arrive in:

1. `RDI`
2. `RSI`
3. `RDX`
4. `RCX`
5. `R8`
6. `R9`

### return value

Put return value in `RAX`.

### callee-saved registers

If your function changes these, restore them before `ret`:

- `RBX`, `RBP`, `R12`, `R13`, `R14`, `R15`

Other general registers are caller-saved.

### stack alignment

Before calling another function, stack should be 16-byte aligned.

For beginner exercises, if your function does not call other functions,
this is usually simpler.

---

## 5) minimal function example

Example: function that returns constant 42.

```asm
global my_answer

section .text
my_answer:
	mov rax, 42
	ret
```

If called from C as `long my_answer(void);`, it returns `42` in `RAX`.

---

## 6) nasm + linker flow (basic)

Typical steps:

1. Assemble `.s` source with `nasm` into an object file.
2. Link object file with C files (or other objects) using your toolchain.

If your project Makefile is already provided, follow it exactly.

---

## 7) beginner checklist

Before testing a function, verify:

- [ ] Function symbol is marked `global`.
- [ ] Function name matches expected C prototype.
- [ ] Arguments read from the right registers (`RDI`, `RSI`, ...).
- [ ] Return value written to `RAX`.
- [ ] Saved registers restored if modified.
- [ ] Syntax is Intel and file is assembled by `nasm`.

If something “almost works”, 80% of the time it is naming, calling convention,
or register clobbering.