%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

extern idt_global_handler
idt_global_pre_handler_asm:

.check_cs:
    ; cs is supposed to be here if an error code
    ; was pushed
    cmp [rsp+24], word 0x08
    je .call_c_global_handler

    ; fake error code
    mov [rsp-16], rax
    mov rax, [rsp]
    mov [rsp-8], rax
    mov qword [rsp], 0
    mov rax, [rsp-16]
    sub rsp, 8

.call_c_global_handler:
    pushaq

    mov rdi, rsp
    add rdi, 16 ; eliminate error code and vector from stack
    mov rsi, [rsp+120] ; vector
    mov rdx, [rsp+128] ; error code
    call idt_global_handler

    popaq
    add rsp, 16
    iretq

; Hardcodes the address of an interrupt thunk
%macro idt_pre_handler_address 1
dq idt_pre_handler_%1
%endmacro

; Hardcodes thr addresses of all 256 interrupt thunks
global idt_pre_handlers
idt_pre_handlers:
%assign i 0
%rep 256
idt_pre_handler_address i
%assign i i + 1
%endrep

; Template for int thunks
%macro idt_pre_handler 1
idt_pre_handler_%1:
    push qword %1
    jmp idt_global_pre_handler_asm
%endmacro

; Creates all 256 int thunks
%assign i 0
%rep 256
idt_pre_handler i
%assign i i + 1
%endrep
