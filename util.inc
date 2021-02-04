%macro  syscall 1-*

%if %0 > 1
  push ebx
  %if %0 > 4
  push esi
  push edi
  %endif
%endif

%rep %0
  %rotate -1
    push dword %1
%endrep

  pop eax
%if %0 > 1
  pop ebx
  %if %0 > 2
  pop ecx
    %if %0 > 3
  pop edx
      %if %0 > 4
  pop esi
        %if %0 > 5
  pop edi
          %if %0 > 6
            %error "Can't handle Linux syscalls for more than 5 params"
          %endif
        %endif
      %endif
    %endif
  %endif
%endif
  int 80h
%if %0 > 1
  %if %0 > 4
  pop edi
  pop esi
  %endif
  pop ebx
%endif
%endmacro

%define SYS_EXIT 1
%define SYS_FORK 2
%define SYS_READ 3
%define SYS_WRITE 4
%define SYS_OPEN 5
%define SYS_CLOSE 6
%define SYS_CREAT 8
%define SYS_UNLINK 9
%define SYS_EXECVE 11
%define SYS_TIME 13
%define SYS_LSEEK 19

%define SIN 0
%define SOUT 1

%define O_RDONLY 0
%define O_WRONLY 1
%define O_RDWR 2

section .data
nlstr    db   10, 0

section .bss
uint_32_string    resb    11

section .text
;------------------------------------------
; int str_len(const char* message)
; String length calculation function
str_len:
        push    ebp
        mov     ebp, esp

        mov     eax, [ebp + 8]
.next_char:
        cmp     byte [eax], 0
        jz      .return
        inc     eax
        jmp     .next_char
.return:
        sub     eax, [ebp + 8]
        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; void str_print(const char* message)
; String printing function
str_print:
        push    ebp
        mov     ebp, esp

        mov     eax, [ebp + 8]
        push    eax
        call    str_len
        add     esp, 4

        push    ebx
        push    edx

        syscall SYS_WRITE, SOUT, [ebp + 8], eax

        pop     edx
        pop     ebx

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; void print_lf()
; Print line feed
print_lf:
        push    ebp
        mov     ebp, esp

        push    dword nlstr
        call    str_print
        add     esp, 4

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; void print_int(int number)
; Print integer number
print_int:
        push    ebp
        mov     ebp, esp

        mov     eax, [ebp + 8]

        push    edx
        push    esi

        mov     ecx, 10
        add     ecx, uint_32_string
        mov     byte [ecx], 0
.divide_loop:
        dec     ecx
        xor     edx, edx
        mov     esi, 10
        idiv    esi                 ; eax -> quotient edx -> remainder
        add     edx, 48
        mov     [ecx], dl
        cmp     eax, 0
        jnz     .divide_loop

        push    ecx
        call    str_print
        call    print_lf
        add     esp, 4

        pop     esi
        pop     edx

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; int atoi(const char* number)
; Ascii to integer (atoi)
atoi:
        push    ebp
        mov     ebp, esp

        push    esi
        push    edx
        push    ebx

        mov     esi, [ebp + 8]          ; number to convert
        xor     ecx, ecx                 ; counter
        xor     eax, eax                 ; result

.multiply_loop:
        xor     ebx, ebx
        mov     edx, 10                  ; radix
        mov     bl, [esi + ecx]         ; current char
        cmp     bl, 48                   ; char code < 48 ('0') ?
        jl      .return
        cmp     bl, 57                  ; char code > 57 ('9') ?
        jg      .return

        mul     edx                     ; eax * edx -> edx:eax
        sub     bl, 48                   ; convert char to number
        add     eax, ebx

        inc     ecx
        jmp     .multiply_loop

.return: 
        pop     ebx
        pop     edx
        pop     esi

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; void read_input(char* buffer, int length)
; Read standard input
read_input:
        push    ebp
        mov     ebp, esp

        push    ebx
        push    edx
        
        mov     edx, [ebp + 12]
        mov     ecx, [ebp + 8]
        mov     ebx, SIN
        mov     eax, SYS_READ
        int     80h

        pop     edx
        pop     ebx

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; int execve(const char* filename, const char *const *argv, const char *const *envp)
; Execute program
execve: 
        push    ebp
        mov     ebp, esp

        push    edx
        push    ebx
        
        mov     edx, [ebp + 16]
        mov     ecx, [ebp + 12]
        mov     ebx, [ebp + 8]
        mov     eax, SYS_EXECVE
        int     80h

        pop     ebx
        pop     edx

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; int fork()
; Fork child process
fork:
        push    ebp
        mov     ebp, esp

        mov     eax, SYS_FORK
        int     80h

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; int time()
; Return Unix Epoch time
time:
        push    ebp
        mov     ebp, esp

        mov     eax, SYS_TIME
        int     80h

        mov     esp, ebp
        pop     ebp
        ret
;------------------------------------------
; void exit()
; Exit program and restore resources
exit:
        push    ebp
        mov     ebp, esp

        mov     ebx, 0
        mov     eax, SYS_EXIT
        int     80h

        mov     esp, ebp
        pop     ebp
        ret
