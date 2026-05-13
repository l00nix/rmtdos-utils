;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

; Implements "int 2f", the "DOS multiplex interrupt", allowing a transient
; instance of our program to communicate with a resident instance.

CPU_REGS_SIZE= $1a                   ; sizeof(struct CpuRegs)

.data
.extern _int2f_original_handler       ; uint32_t (segment:offset)
.global _int2f_stack_bottom, _int2f_stack_top

_int2f_stack_bottom:
    .blkb  $80
_int2f_stack_top:

int2f_saved_ss:
    .word $0
int2f_saved_sp:
    .word $0

.text
; // void int2f_handler(struct CpuRegs *regs)
.extern _int2f_handler

.global _int2f_isr

int2f_chain_previous:
    popf
    seg     cs
    jmpf    [_int2f_original_handler]

_int2f_isr:

; Is this request for us?  If yes, call our C-language handler.
; If no, then chain to the previous handler.
    pushf
    cmp     ax, #$fb00               ; Magic signature
    jne     int2f_chain_previous
    cmp     bx, #$5a98               ; Magic signature
    jne     int2f_chain_previous

; Signature matches, so call our function.
    popf

; Switch to our private stack.
    seg     cs
    mov     int2f_saved_ss, ss
    seg     cs
    mov     int2f_saved_sp, sp

    mov     di, cs                   ; Need a scratch register...
    mov     ss, di
    lea     sp, _int2f_stack_top - 2

; Reserve stack space for 'struct CpuRegs regs'.
    push    bp
    sub     sp, #CPU_REGS_SIZE
    mov     bp, sp

; Populate 'regs'
    mov     [bp + 0], ax             ; regs.ax
    mov     [bp + 2], bx             ; regs.bx
    mov     [bp + 4], cx             ; regs.cx
    mov     [bp + 6], dx             ; regs.dx
    mov     [bp + 8], si             ; regs.si
    mov     [bp + 10], di            ; regs.di
    mov     [bp + 12], sp            ; regs.sp
    mov     [bp + 14], bp            ; regs.bp

    push    ax
    popf
    mov     [bp + 16], ax            ; regs.flags
    mov     [bp + 18], cs            ; regs.cs
    mov     [bp + 20], ds            ; regs.ds
    mov     [bp + 22], es            ; regs.es
    mov     [bp + 24], ss            ; regs.ss

; Save BP in case callee fails to preserve them.
    push    bp

; Point to our (resident) segment.
    mov     ax, cs
    mov     ds, ax
    mov     es, ax

; Call C-language handler.
; extern void int2f_handler(struct CpuRegs *regs)
    push    bp                       ; arg0, '&regs'
    call    _int2f_handler
    add     sp, #2

; Restore saved critical registers.
    pop     bp

; Extract some outbound registers from C function.
; Do NOT update BP, SP, CS
    mov     ax, [bp + 16]
    push    ax
    popf

    mov     ax, [bp + 0]
    mov     bx, [bp + 2]
    mov     cx, [bp + 4]
    mov     dx, [bp + 6]
    mov     si, [bp + 8]
    mov     di, [bp + 10]
    mov     ds, [bp + 20]
    mov     es, [bp + 22]

; Clean our private stack (not really needed?)
    add     sp, #CPU_REGS_SIZE
    pop     bp

; Switch back to the caller's stack.
    seg     cs
    mov     ss, int2f_saved_ss
    seg     cs
    mov     sp, int2f_saved_sp

    iret
