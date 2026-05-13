;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

; Implements "int 28", our "DOS Idle" interrupt handler.  This is called when
; DOS is "idle, waiting for user input".  It is safe to make some DOS (int 21)
; calls during this time.

.data
.extern _int28_original_handler            ; uint32_t (segment:offset)
.global _int28_stack_bottom, _int28_stack_top

_int28_stack_bottom:
    .blkb    $100
_int28_stack_top:

int28_saved_ss:
    .word $0
int28_saved_sp:
    .word $0

.text
; // void int28_handler()
.extern _int28_handler

.global _int28_isr
_int28_isr:
; Save AX and flags on existing stack.
  pushf
  push   ax

; Save previous SS:SP
  seg    cs
  mov    int28_saved_ss, ss
  seg    cs
  mov    int28_saved_sp, sp

; Switch to our private stack
  mov    ax, cs
  mov    ss, ax
  lea    sp, _int28_stack_top - 2

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
  call   _int28_handler

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
  mov    ss, int28_saved_ss
  seg    cs
  mov    sp, int28_saved_sp

  pop    ax
  popf

; Call original int28 handler.
  seg    cs
  jmpf   [_int28_original_handler]
