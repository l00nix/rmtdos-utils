;  SPDX-FileCopyrightText: 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
;  SPDX-License-Identifier: GPL-2.0-or-later

; // void x86_dos_idle();
.global _x86_dos_idle
_x86_dos_idle:
  int    $28
  ret
