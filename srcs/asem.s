bits 64
global _start

hello_world:
	db "Hello, World!", 10, 0

section .text
_start:
	; sys_write
	mov rax, 1
	mov rdi, 1
	lea rsi, [hello_world]
	mov rdx, 14
	syscall
	; sys_exit
	mov rax, 60
	mov rdi, 69
	syscall
