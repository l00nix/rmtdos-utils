/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* BCC is weird.  "const" does not exist, so we must include stdlib.h to
   get the macro that makes "const" go away. */
#include <stdlib.h>

#include "lib16/hex2int.h"

uint16_t hex_to_uint16(const char *str) {
  uint16_t r = 0;
  uint16_t v;

  for (; *str; ++str) {
    if ((*str >= '0') && (*str <= '9')) {
      v = *str - '0';
    } else if ((*str >= 'A') && (*str <= 'F')) {
      v = *str - 'A' + 10;
    } else if ((*str >= 'a') && (*str <= 'f')) {
      v = *str - 'a' + 10;
    } else {
      break;
    }

    r = (r << 4) | v;
  }

  return r;
}
