;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

; Implements "int 08", "BIOS Timer" interrupt handler (~18.2065/s).

.data
.extern _int08_original_handler            ; uint32_t (segment:offset)
.global _int08_stack_bottom, _int08_stack_top

_int08_stack_bottom:
    .blkb    $200
_int08_stack_top:

int08_saved_ss:
    .word $0
int08_saved_sp:
    .word $0

.text
; // void int08_handler()
.extern _int08_handler

.global _int08_isr
_int08_isr:
; Save AX and flags on existing stack.
  pushf
  push   ax

; Save previous SS:SP
  seg    cs
  mov    int08_saved_ss, ss
  seg    cs
  mov    int08_saved_sp, sp

; Switch to our private stack
  mov    ax, cs
  mov    ss, ax
  lea    sp, _int08_stack_top - 2

; Save all registers (except AX, already saved on original stack).
  push   bx
  push   cx
  push   dx
  push   si
  push   di
  push   bp
  push   ds
  push   es

  mov    ds, ax
  mov    es, ax

; Call C-language function.  Don't formally pass registers to it.
  call   _int08_handler

  pop    es
  pop    ds
  pop    bp
  pop    di
  pop    si
  pop    dx
  pop    cx
  pop    bx

; Restore original stack.
  seg    cs
  mov    ss, int08_saved_ss
  seg    cs
  mov    sp, int08_saved_sp

  pop    ax
  popf

; Call original int08 handler.
  seg    cs
  jmpf   [_int08_original_handler]
