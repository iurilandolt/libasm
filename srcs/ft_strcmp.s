bits 64
global ft_strcmp

section .text
ft_strcmp:

.loop:
    movzx eax, byte [rdi] ; load byte from s1, zero-extended
    movzx ecx, byte [rsi] ; load byte from s2, zero-extended
    cmp eax, ecx          ; are they equal?
    jne .done             ; if not, compute difference
    cmp eax, 0            ; are we at the null terminator?
    je  .done             ; if so, strings are equal (eax - ecx = 0)
    inc rdi               ; s1++
    inc rsi               ; s2++
    jmp .loop

.done:
    sub eax, ecx          ; s1[i] - s2[i]
    jl  .neg              ; negative → return -1
    jg  .pos              ; positive → return  1
    ret                   ; zero    → return  0 (eax already 0)

.neg:
    mov eax, -1
    ret
	
.pos:
    mov eax, 1
    ret

section .note.GNU-stack progbits

; PROTOTYPE
;   int strcmp(const char *s1, const char *s2);
;
; DESCRIPTION
;   The strcmp() function compares the two strings s1 and s2.
;   The comparison is done using unsigned characters.
;   Returns 0 if equal, negative if s1 < s2, positive if s1 > s2.
;
; RETURN VALUE
;   Returns -1, 0, or 1 if s1 is less than, equal to, or greater than s2.
;   POSIX only guarantees sign — we normalize to match common libc behaviour.
;
; NOTE
;   An alternative uses register aliasing — accessing sub-parts of a register
;   by name (AL = low byte of RAX, CL = low byte of RCX, etc.).
;   That approach loads into AL/CL, then requires a separate movzx before the
;   subtraction to clear the upper bits of RAX/RCX.
;
;   .loop:
;       mov al, byte [rdi]    ; load byte from s1 into low byte of rax
;       mov cl, byte [rsi]    ; load byte from s2 into low byte of rcx
;       cmp al, cl
;       jne .done
;       cmp al, 0
;       [ ... ]
;   .done:
;       movzx eax, al         ; must zero-extend before subtracting
;       movzx ecx, cl
;       [ ... ]
