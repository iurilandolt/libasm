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