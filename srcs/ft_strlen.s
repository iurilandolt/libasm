bits 64
global ft_strlen

section .text
ft_strlen:
    xor rax, rax              ; counter = 0

.loop:
    cmp byte [rdi + rax], 0   ; is str[counter] == null?
    je  .done
    inc rax                   ; counter++
    jmp .loop

.done:
    ret                       ; return counter in rax

section .note.GNU-stack progbits

; PROTOTYPE
;   size_t strlen(const char *s);
;
; DESCRIPTION
;   The strlen() function calculates the length of the string pointed to by
;   s, excluding the terminating null byte ('\0').
;
; RETURN VALUE
;   The strlen() function returns the number of bytes in the string pointed
;   to by s.
