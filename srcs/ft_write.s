bits 64
global ft_write
extern __errno_location

section .text
ft_write:
    mov rax, 1            ; sys_write
    syscall               ; rdi=fd, rsi=buf, rdx=count already in place

    cmp rax, 0            ; negative = error
    jl .error
    ret                   ; success: return bytes written in rax

.error:
    neg rax               ; flip to positive errno code
    push rax              ; save it — call below will clobber rax
    call __errno_location wrt ..plt ; rax = pointer to errno
    pop rcx               ; retrieve saved errno code
    mov [rax], rcx        ; *errno = error code
    mov rax, -1           ; return -1 to caller
    ret

section .note.GNU-stack progbits

; PROTOTYPE
;   ssize_t write(int fd, const void *buf, size_t count);
;
; DESCRIPTION
;   write() writes up to count bytes from the buffer starting at buf
;   to the file referred to by the file descriptor fd.
;
; RETURN VALUE
;   On success, the number of bytes written is returned.
;   On error, -1 is returned and errno is set.
