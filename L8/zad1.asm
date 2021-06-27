
section .data
input_number     db 254; input decimal number
table db "0123456789ABCDEF"
section .bss           ;Uninitialized data
   num resb 2
section .text
        global  _start

_start:
        mov al, [input_number]
        mov bl, 16
        div bl
        mov BYTE[num], al  ; in al is stored integer part 
        mul bl
        mov al, [input_number]
        and al, 0xF
        ;xor al, [input_number]
        mov BYTE[num+4], al
        mov ecx, table
        add ecx, [num]
        mov eax, 4
        mov ebx, 1
        mov edx, 1
        int 80h 
        mov ecx, table
        add ecx, [num+4]
        mov eax, 4
        mov ebx, 1
        mov edx, 1
        int 80h 
      


; wyjscie z programu
        mov     eax, 1
        xor     ebx, ebx
        int     0x80
; KONIEC PROGRAMU
