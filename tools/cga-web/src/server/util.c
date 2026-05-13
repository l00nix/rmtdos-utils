/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>

#include "server/util.h"

char *fmt_mac_addr(char *dest, const uint8_t *mac_addr) {
  sprintf(dest, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
          mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  return dest;
}
