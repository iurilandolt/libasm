bits 64
global ft_calloc
extern malloc
extern __errno_location

ENOMEM equ 12

section .text
ft_calloc:
    mov rax, rdi           ; rax = nmemb
    mul rsi                ; rdx:rax = nmemb * size — CF set if it overflowed
    jc .overflow           ; product doesn't fit in 64 bits

    push rax               ; save total size — malloc returns in rax
    mov rdi, rax           ; malloc argument = nmemb * size
    call malloc wrt ..plt  ; rax = new buffer, or NULL on failure
    pop rcx                ; rcx = total size, for the zeroing loop below

    test rax, rax          ; malloc returned NULL?
    je .done               ; yes — malloc already set errno

    mov rdx, rax           ; park the pointer; rep stosb destroys rdi and rcx
    mov rdi, rax           ; dest = new buffer
    xor eax, eax           ; al = 0, the fill byte
    rep stosb              ; write rcx zero bytes at [rdi]
    mov rax, rdx           ; return the buffer

.done:
    ret

.overflow:
    sub rsp, 8             ; align the stack — no push on this path
    call __errno_location wrt ..plt ; rax = pointer to errno
    mov dword [rax], ENOMEM ; *errno = ENOMEM, like glibc does
    add rsp, 8
    xor eax, eax           ; return NULL
    ret

section .note.GNU-stack progbits

; PROTOTYPE
;   void *calloc(size_t nmemb, size_t size);
;
; DESCRIPTION
;   calloc() allocates memory for an array of nmemb elements of size bytes
;   each and returns a pointer to the allocated memory. The memory is set
;   to zero.
;
; RETURN VALUE
;   On success, a pointer to the allocated memory.
;   On error, NULL. If nmemb * size would overflow size_t, NULL is returned
;   and errno is set to ENOMEM, without malloc ever being called.
;
; NOTE
;   MUL sets CF and OF when the upper half of the product (rdx) is non-zero,
;   which is exactly the overflow the subject asks us to detect ourselves.
;   That is why a single jc replaces any manual division check.
;
;   REP STOSB writes AL to [RDI] and increments RDI, RCX times. It relies on
;   the direction flag being clear, which the ABI guarantees on entry.
