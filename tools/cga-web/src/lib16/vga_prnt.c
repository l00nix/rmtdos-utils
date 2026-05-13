/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdarg.h>
#include <stdio.h>

#include "lib16/vga.h"

// 1 line of 80-col text, plus "\0", rounded up.
#define BUF_LEN 82

int vga_printf(int x, int y, uint8_t attr, const char *fmt, ...) {
  va_list ap;
  int r;
  char tmp[BUF_LEN];

  va_start(ap, fmt);
  r = vsprintf(tmp, fmt, ap);
  va_end(ap);

  vga_write_str(x, y, attr, tmp);

  return r;
}
