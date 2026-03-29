
bits 64
global ft_strcpy

section .text
ft_strcpy:
    mov rax, rdi          ; save dest — we must return it

.loop:
    mov cl, byte [rsi]    ; load byte from src
    mov byte [rdi], cl    ; write it to dest
    inc rsi               ; src++
    inc rdi               ; dest++
    cmp cl, 0             ; was the byte we just copied null?
    jne .loop             ; if not, keep going

    ret                   ; rax still holds original dest

; PROTOTYPE
;   char *strcpy(char *restrict dst, const char *restrict src);
;
; DESCRIPTION
;   strcpy() copies the string pointed to by src, into a string at the buffer
;   pointed to by dst. The programmer is responsible for allocating a
;   destination buffer large enough, that is, strlen(src) + 1.
;
; RETURN VALUE
;   strcpy() returns dst.
;
; NOTE
;   CL is register aliasing in action — it is the low byte of RCX, used here
;   as a 1-byte scratch register to ferry one character at a time between src
;   and dest. RCX is caller-saved so no push/pop needed.
