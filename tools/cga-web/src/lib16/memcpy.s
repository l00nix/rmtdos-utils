;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

#if __FIRST_ARG_IN_AX__
  #error __FIRST_ARG_IN_AX__ not supported
#endif

; // void x86_memcpy_bytes(
; //  uint16_t dest_segment,
; //  uint16_t dest_offset,
; //  uint16_t src_segment,
; //  uint16_t src_offset,
; //  uint16_t byte_count
; // );
.global _x86_memcpy_bytes
_x86_memcpy_bytes:
  push    bp
  mov     bp, sp
  push    ds
  push    es

  mov     es, [bp + 4]          ; dest_segment
  mov     di, [bp + 6]          ; dest_offset
  mov     ds, [bp + 8]          ; src_segment
  mov     si, [bp + 10]         ; src_offset
  mov     cx, [bp + 12]         ; byte_count

  rep
  movsb

  pop     es
  pop     ds
  pop     bp
  ret
