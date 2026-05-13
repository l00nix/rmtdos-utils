;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

; Implements packet receive handler
;
; Craziness: Our program is compiled by 'bcc' for the Tiny memory model.
; That is, MS-DOS "COM" programs where cs=ds=es=ss.  The code emitted by the
; compiler assumes this, and takes shortcuts when referencing stack variables.
; However, the Packet driver freely invokes our callback with its own SS and DS
; registers often to to its PSP (program segment prefix).  For us to
; successfully call back into 'C' from the handler, we must switch to an
; alternate stack.  However, we do not know what our program's SP was when
; the NIC handled a packet interrupt and caused the packet driver to invoke us.

.data
pktdrv_stack_bottom:
    .blkb    $100
pktdrv_stack_top:

pktdrv_saved_ss:
    .word  $0
pktdrv_saved_sp:
    .word  $0

CPU_REGS_SIZE= $1a                   ; sizeof(struct CpuRegs)

.text
.extern _pktdrv_receive_func
entry _pktdrv_receive_isr
; Per mbbrutman@gmail.com, the driver is not re-entrant, so we can safely use
; global to stash our original stack and a single global alt stack.

; On entry:
;   AX = flag
;   BX = handle
;   CX = length of buffer
;   DS:SI = buffer address
;   SS:SP = Packet driver's stack
;
; On exit:
;   ES:DI = our buffer that packet driver should copy packet into.


_pktdrv_receive_isr:
; Save previous stack SS:SP
    seg     cs                       ; Our .text segment.
    mov     pktdrv_saved_ss, ss
    seg     cs
    mov     pktdrv_saved_sp, sp

; Tell CPU to use our private stack with SS=CS.
    mov     dx, cs                   ; Caller does not use DX.
    mov     ss, dx
    lea     sp, pktdrv_stack_top - 2

; We are now on our alternate stack.  Proceed as normal.
    push    bp
    sub     sp, #CPU_REGS_SIZE       ; Allocate local variable 'CpuRegs'.
    mov     bp, sp

    mov     [bp + 0], ax             ; regs.ax, flag (0 or 1)
    mov     [bp + 2], bx             ; regs.bx, handle
    mov     [bp + 4], cx             ; regs.cx, len

    xor     ax, ax
    mov     [bp + 6], ax             ; regs.dx (unused)
    mov     [bp + 8], si             ; regs.si (buffer offset)
    mov     [bp + 10], di            ; regs.di (only used in return value)
    mov     [bp + 12], sp            ; regs.sp (unused by driver)
    mov     [bp + 14], bp            ; regs.bp (unused by driver)
    mov     [bp + 16], ax            ; regs.flags (unused by driver)
    mov     [bp + 18], cs            ; regs.cs
    mov     [bp + 20], ds            ; regs.ds (buffer segment)
    mov     [bp + 22], es            ; regs.es (buffer segment)
    mov     [bp + 24], ss            ; regs.ss

    push    bp                       ; Save BP in case caller munges it.
    push    ds

;   Need to set 'ds' to our own segment.
    push    cs
    pop     ds                       ; Pop our CS into DS.

;   Call C-language handler
;   extern void pktdrv_receive_func(struct CpuRegs *regs);
    push    bp                       ; arg0, 'regs'
    call    _pktdrv_receive_func
    add     sp, #2

    pop     ds
    pop     bp

; Return es:di to the driver.  Other regs should not matter?
    mov     ax, [bp + 0]
    mov     bx, [bp + 2]
    mov     cx, [bp + 4]
    mov     dx, [bp + 6]
    mov     si, [bp + 8]
    mov     di, [bp + 10]
    mov     ds, [bp + 20]
    mov     es, [bp + 22]

    add     sp, #CPU_REGS_SIZE
    pop     bp

; Restore the packet driver's original stack.
    seg     cs
    mov     ss, pktdrv_saved_ss
    seg     cs
    mov     sp, pktdrv_saved_sp

    retf


; Probes IRQ handlers looking for a valid packet driver.
; Returrns first IRQ # in AL that has a valid packet driver, or 0.
MIN_IRQ EQU 0x60
MAX_IRQ EQU 0x80
SENTINEL_LEN EQU 8

.text
pktdrv_sentinel:
    .ascii "PKT DRVR"

; // PktDrvIrq pktdrv_probe_all();
.global _pktdrv_probe_all
_pktdrv_probe_all:
  push     bp
  push     ds
  push     es
  push     si
  push     di

  mov      ax, cs
  mov      es, ax

  mov      dx, #MIN_IRQ            ; DL = irq to probe
pktdrv_probe_loop:
  lea      di, pktdrv_sentinel     ; ES:DI set for cmpsb

  xor      ax, ax
  mov      ds, ax                  ; DS = $0000 (irq table)

  mov      bx, dx                  ; BL = irq to probe
  shl      bx, 2                   ; BX = irq offset (4 bytes each)

  mov      si, [bx]                ; SI = ISR offset
  mov      ds, [bx + 2]            ; DS:DI = ISR entry
  add      si, #3                  ; "PKT DRVR" occurs 3 bytes in.

  mov      cx, #SENTINEL_LEN
  rep
  cmpsb
  je       pktdrv_probe_match
  inc      dx
  cmp      dx, #MAX_IRQ
  jb       pktdrv_probe_loop

; If we get here, none of the IRQs point to a packet driver.
  xor      dx, dx                  ; Reset IRQ (loop var) to zer0

pktdrv_probe_match:
  mov      ax, dx                  ; DX = IRQ we're probing (0 on fail).

pktdrv_probe_done:
  pop      di
  pop      si
  pop      es
  pop      ds
  pop      bp
  ret
