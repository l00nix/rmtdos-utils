/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "lib16/types.h"

#ifndef offsetof
#define offsetof(type, member) ((size_t) & ((type *)0)->member)
#endif

#define ALIGN_TO_SEGMENT(ptr) (void *)(((uint16_t)(ptr) + 15) & 0xfff0)

// Treats segment:offset as 32-bit int, same as __getvect() and __setvect().
#define MK_FP(seg, off) (((uint32_t)(seg) << 16) | (uint32_t)(off))

// Asm routines for byte swapping.
extern uint16_t htons(uint16_t x);
#define ntohs(x) htons(x)

extern uint32_t htonl(uint32_t x);
#define ntohl(x) htonl(x)

// Returns non-zeor if the pointed-to MAC address is a 'broadcast' address.
// That is, "ff:ff:ff:ff:ff:ff".
extern int is_broadcast_mac_addr(const uint8_t *mac_addr);

// Space-optimized 6-byte memcpy().
extern void copy_mac_addr(uint8_t *dest, const uint8_t *src);

// Char buffer size required to snprintf() a MAC address.
#define MAC_ADDR_FMT_LEN (5 + 6 * 2 + 1)

extern char *fmt_mac_addr(char *dest, const uint8_t *mac_addr);
