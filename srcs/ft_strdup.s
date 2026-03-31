bits 64
global ft_strdup
extern malloc
extern ft_strlen
extern ft_strcpy

section .text
ft_strdup:
    test rdi, rdi          ; NULL input?
    je .null               ; return NULL immediately

    push rbx               ; callee-saved — we'll use it to hold src across calls
    mov rbx, rdi           ; save src: both ft_strlen and malloc will need rdi

    call ft_strlen         ; rax = strlen(src) — rdi is unchanged by ft_strlen
    inc rax                ; +1 for the null terminator

    mov rdi, rax           ; malloc argument = len + 1
    call malloc wrt ..plt  ; rax = new buffer, or NULL on failure

    test rax, rax          ; malloc returned NULL?
    je .done               ; yes — return NULL

    mov rdi, rax           ; dest = new buffer
    mov rsi, rbx           ; src  = original string
    call ft_strcpy         ; copy src into dest; rax = dest (our new buffer)

.done:
    pop rbx
    ret

.null:
    xor rax, rax           ; return NULL
    ret

section .note.GNU-stack progbits

; PROTOTYPE
;   char *strdup(const char *s);
;
; DESCRIPTION
;   strdup() returns a pointer to a new string which is a duplicate of the
;   string s. Memory for the new string is obtained with malloc(3).
;
; RETURN VALUE
;   On success, a pointer to the duplicated string.
;   On error (malloc failure), NULL.
