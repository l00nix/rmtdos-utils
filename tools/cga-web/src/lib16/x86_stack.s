;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

#if __FIRST_ARG_IN_AX__
  #error __FIRST_ARG_IN_AX__ not supported
#endif


; // void* x86_move_stack(void* tos_offset);
; Caveats!
; 1. Only safe to call from 'main()'.
; 2. Do not take the address of anything on main's stack frame.
; 3. Must be called with an EVEN argument!
;
; Assumes the following on entry:
; 1. That current top of stack is 0x0000.
; 2. CS=DS=ES=SS.
; 3. SS:SP is current bottom of stack.
; 4. SS:BP is past end of main's locals.
; 5. There is no data past the top of the new stack.
;
; BCC's "crt0" seems fine with when we return from "main()".  From what I
; can tell, "crt0" does not "push bp" or store (old) stack frame locals in
; any locals or globals.

.text
.global _x86_move_stack
_x86_move_stack:
; NOTE: We do NOT 'push bp' here, so formal arg stack offsets are special.
  mov    cx, sp
  neg    cx                          ; CX = size of current stack.

  mov    bx, sp
  mov    di, [bx + 2]                ; ES:DI = top of new stack.
  and    di, #$fffe                  ; Ensure top of new stack is EVEN.
  sub    di, cx                      ; ES:DI = bottom of new stack.
  mov    si, sp                      ; DS:SI = bottom of old stack.

  mov    dx, si
  sub    dx, di                      ; DX = Distance stack is moved.

  cld
  rep
  movsb                              ; memcpy() the stack.

  pushf                              ; Save flags and disable interrupts.
  pop    ax
  cli

  sub    sp, dx                      ; Adjust SP and BP to point to same logical
  sub    bp, dx                      ; location in the new stack.

  push   ax                          ; Restore previous CPU flags.
  popf

  ; Clear from new top of stack to end of the segment.  This is not strictly
  ; necessary, but aides in debugging.
  mov    bx, sp
  mov    di, [bx + 2]                ; BX = top of new stack.
  push   di                          ; Save as return value.

  mov    cx, di
  neg    cx                          ; CX = count of bytes above TOS.
  shr    cx, 1                       ; CX = count of words above TOS.

  mov    ax, #$ddbf                  ; 16-bit "0xdeadbeef".
  rep
  stosw

  pop    ax                          ; AX = top of stack (return value).
  ret
