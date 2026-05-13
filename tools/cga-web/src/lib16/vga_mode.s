;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

; // void vga_mode_80x25();
.text
.global _vga_mode_80x25
_vga_mode_80x25:
  pushf
  cli

  mov     ax, #$0002
  xor     bx, bx
  int     $10

  mov     ax, #$1201         ; Set 200 scan line mode.
  mov     bx, #$0030         ; Select scan lines for TEXT mode.
  int     $10

  popf
  ret


; // TODO: Buggy, sets mode to 80x28, not 80x43.
; Only works on VGA (for pure EGA, use different code).
; // void vga_mode_80x43();
.text
.global _vga_mode_80x43
_vga_mode_80x43:
  pushf
  cli

  mov     ax, #$1201         ; Set 350 scan line mode.
  mov     bx, #$0030         ; Select scan lines for TEXT mode.
  int     $10

  mov     ax, #$0003         ; Set 80x25 color text mode.
  xor     bx, bx
  int     $10

  mov     ax, #$1112         ; Set 8x8 font (changes mode to 80x43)
  xor     bx, bx             ; Table #0
  int     $10

  mov     ax, #$1200         ; Use alternate print-screen.
  mov     bx, #$0020
  int     $10

  popf
  ret


; // void vga_mode_80x50();
.text
.global _vga_mode_80x50
_vga_mode_80x50:
  pushf
  cli

  mov     ax, #$1202         ; Set 400 scan line mode.
  mov     bx, #$0030         ; Select scan lines for TEXT mode.
  int     $10

  mov     bx, #$30           ; Unused?
  mov     ax, #$3            ; Set 80x25 color text mode.
  int     $10

  mov     ax, #$1112         ; Set 8x8 font (gives 80x50 text mode)
  mov     bx, #$0            ; Table #0
  int     $10

  popf
  ret
