/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// Routines for accessing a Crynwr Packet Driver.
//
// http://crynwr.com/packet_driver.html

#ifndef __RMTDOS_SERVER_PKTDRV_H
#define __RMTDOS_SERVER_PKTDRV_H

#include "common/ethernet.h"
#include "lib16/types.h"

// No idea what the real maxlen is.  We'll truncate at this size.
#define PKTDRV_NAME_MAXLEN 64

// Technically, IRQs are 8-bits (uint8_t)...
typedef uint16_t PktDrvIrq;

// Most methods require a 16-bit opaque "handle".
typedef uint16_t PktDrvHandle;

// http://crynwr.com/packet_driver.html, Appendix A
enum PktDrvClass {
  PKTDRV_CLASS_ETHERNET = 1, // "Regular Etheret" (you want this)
  PKTDRV_CLASS_PRONET10 = 2,
  PKTDRV_CLASS_8025 = 3, // Token Ring
  PKTDRV_CLASS_OMNINET = 4,
  PKTDRV_CLASS_APPLETALK = 5,
  PKTDRV_CLASS_CLARKSON_SLIP = 6,
  PKTDRV_CLASS_STARLAN = 7,
  PKTDRV_CLASS_ARCNET = 8,
  PKTDRV_CLASS_AX25 = 9,
  PKTDRV_CLASS_KISS = 10,
  PKTDRV_CLASS_8022 = 11,
  PKTDRV_CLASS_FDDI = 12,
  PKTDRV_CLASS_X25 = 13,
  PKTDRV_CLASS_NTLANSTAR = 14,
};

// Passed in AH register when calling the driver's irq.
// http://crynwr.com/packet_driver.html, Appendix B.
enum PktDrvFunctions {
  PKTDRV_FUNC_DRIVER_INFO = 1,
  PKTDRV_FUNC_ACCESS_TYPE = 2,
  PKTDRV_FUNC_RELEASE_TYPE = 3,
  PKTDRV_FUNC_SEND_PKT = 4,
  PKTDRV_FUNC_TERMINATE = 5,
  PKTDRV_FUNC_GET_ADDRESS = 6,
  PKTDRV_FUNC_RESET_INTERFACE = 7,
  PKTDRV_FUNC_GET_PARAMS = 10,         // "high-performance"
  PKTDRV_FUNC_AS_SEND_PKT = 11,        // "high-performance"
  PKTDRV_FUNC_SET_RCV_MODE = 20,       // "extended"
  PKTDRV_FUNC_GET_RCV_MODE = 21,       // "extended"
  PKTDRV_FUNC_SET_MULTICAST_LIST = 22, // "extended"
  PKTDRV_FUNC_GET_MULTICAST_LIST = 23, // "extended"
  PKTDRV_FUNC_GET_STATISTICS = 24,     // "extended"
  PKTDRV_FUNC_SET_ADDRESS = 25,        // "extended"
};

// http://crynwr.com/packet_driver.html, Appendix C.
enum PktDrvResultCode {
  PKTDRV_OK = 0,
  PKTDRV_ERR_BAD_HANDLE = 1,
  PKTDRV_ERR_NO_CLASS = 2,
  PKTDRV_ERR_NO_TYPE = 3,
  PKTDRV_ERR_NO_NUMBER = 4,
  PKTDRV_ERR_BAD_TYPE = 5,
  PKTDRV_ERR_NO_MULTICAST = 6,
  PKTDRV_ERR_CANT_TERMINATE = 7,
  PKTDRV_ERR_BAD_MODE = 8,
  PKTDRV_ERR_NO_SPACE = 9,
  PKTDRV_ERR_TYPE_INUSE = 10,
  PKTDRV_ERR_BAD_COMMAND = 11,
  PKTDRV_ERR_CANT_SEND = 12,
  PKTDRV_ERR_CANT_SET = 13,
  PKTDRV_ERR_BAD_ADDRESS = 14,
  PKTDRV_ERR_CANT_RESET = 15,
  PKTDRV_ERR_NO_DRIVER = 16, // Not in the spec; this is OUR error
};

// http://crynwr.com/packet_driver.html, Section 6.3, "driver_info()"
struct PktDrvInfo {
  uint16_t version;                  // BX
  uint16_t type;                     // DX
  uint8_t _class;                    // CH
  uint8_t number;                    // CL
  uint8_t functionality;             // AL
  uint8_t mac_addr[ETH_ALEN];        // Comes from 6.8, `get_address()`.
  char name[PKTDRV_NAME_MAXLEN + 1]; // DS:SI
};

// http://crynwr.com/packet_driver.html, Section 6.10, "get_parameters()"
// See source doc for specific details.
struct PktDrvParams {
  uint8_t major_rev;
  uint8_t minor_rev;
  uint8_t length;
  uint8_t addr_len;
  uint16_t mtu;
  uint16_t multicast_aval;
  uint16_t rcv_bufs;
  uint16_t xmt_bufs;
  uint16_t int_num;
};

// Section 6.10, `get_parameters()`
extern enum PktDrvResultCode pktdrv_get_parameters(PktDrvIrq irq,
                                                   struct PktDrvParams *params);

// http://crynwr.com/packet_driver.html, Section 4.
// Probe interrupts 0x60 through 0x80 looking for installed packet driver.
// Returns first interrupt number with a valid driver, or 0 if none found.
// Implemented in server/pktrecv.s
extern PktDrvIrq pktdrv_probe_all();

struct PktDrvStats {
  uint32_t packets_recv;
  uint32_t packets_dropped;
  uint32_t packets_sent;
};

// Initializes state and connects to the packet driver.
// If `irq` is zero, will then probe for the packet driver.
// If `pktdrv_init()` returns `PKTDRV_OK`, then the program MUST call
// `pktdrv_done()` on exit, or the packet driver will be left connected to
// the now no-longer resident program.
extern enum PktDrvResultCode pktdrv_init(PktDrvIrq irq);

extern enum PktDrvResultCode pktdrv_done();

// http://crynwr.com/packet_driver.html, Section 6.6
// Caller must fill in packet entirely.
// `length` is the TOTAL BYTE SIZE of the entire ethernet frame, minus
// the trailing CRC (which the packet driver fills in for us).
extern enum PktDrvResultCode pktdrv_send(const void *buffer, uint16_t length);

#endif // __RMTDOS_SERVER_PKTDRV_H
