/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// GCC on Linux supplies many header files which define canoncail ethernet
// structs and constants (defines).  BCC had none of this.

#ifndef __RMTDOS_COMMON_ETHERNET_H
#define __RMTDOS_COMMON_ETHERNET_H

#define ETHERTYPE_RMTDOS (uint16_t)0x80ab

#ifdef __GNUC__

#include <net/ethernet.h>

#else // __GNUC__

// TODO: Replace our `EthernetHeader` with `ether_header`.
#define ether_header EthernetHeader

// 6 octets (bytes) in a IEEE Mac Address.
#define ETH_ALEN 6

// Max bytes in normal ethernet frame.
#define ETH_FRAME_LEN 1514

#endif // __GNUC__

#endif // __RMTDOS_COMMON_ETHERNET_H
