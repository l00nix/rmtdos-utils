/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// Misc utilities.

#ifndef __RMTDOS_CLIENT_UTIL_H_
#define __RMTDOS_CLIENT_UTIL_H_

#include <stddef.h>
#include <stdint.h>

// Char buffer size required to snprintf() a MAC address.
#define MAC_ADDR_FMT_LEN (5 + 6 * 2 + 1)

extern char *fmt_mac_addr(char *dest, size_t max_len, const uint8_t *mac_addr);

#endif // __RMTDOS_CLIENT_UTIL_H_
