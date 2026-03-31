bits 64
global ft_read
extern __errno_location

section .text
ft_read:
    mov rax, 0             ; sys_read
    syscall                ; rdi=fd, rsi=buf, rdx=count already in place

    cmp rax, 0             ; negative = error
    jl .error
    ret                    ; success: return bytes read in rax

.error:
    neg rax                ; flip to positive errno code
    push rax               ; save it — call below will clobber rax
    call __errno_location wrt ..plt ; rax = pointer to errno
    pop rcx                ; retrieve saved errno code
    mov [rax], rcx         ; *errno = error code
    mov rax, -1            ; return -1 to caller
    ret

section .note.GNU-stack progbits

; PROTOTYPE
;   ssize_t read(int fd, void *buf, size_t count);
;
; DESCRIPTION
;   read() attempts to read up to count bytes from file descriptor fd
;   into the buffer starting at buf.
;
; RETURN VALUE
;   On success, the number of bytes read is returned.
;   On error, -1 is returned and errno is set.
