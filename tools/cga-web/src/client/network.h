/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __RMTDOS_CLIENT_NETWORK_H
#define __RMTDOS_CLIENT_NETWORK_H

#include <linux/if_ether.h>
#include <stdint.h>

#include "common/protocol.h"

struct RawSocket {
  int sock_fd;               // Socket
  int if_index;              // Interface index
  uint8_t if_addr[ETH_ALEN]; // Our MAC address
  uint16_t ethertype;
  const char *if_name;

  // Our protocol header (see common/protocol.h)
  uint32_t session_id;
};

// Returns error-like values: 0 on success, <0 on error, >0 (socket) on
// success.
int create_socket(struct RawSocket *result, const char *if_name,
                  uint16_t ethertype);

void close_socket(struct RawSocket *sock);

int send_packet(struct RawSocket *sock, const uint8_t *dest_mac_addr,
                enum PKT_TYPE pkt_type, const void *payload,
                size_t payload_len);

int send_ping(struct RawSocket *sock, const uint8_t *dest_mac_addr);

int send_status_req(struct RawSocket *sock, const uint8_t *dest_mac_addr);

int send_session_start(struct RawSocket *sock, const uint8_t *dest_mac_addr);

int send_keystrokes(struct RawSocket *sock, const uint8_t *dest_mac_addr,
                    size_t count, const struct Keystroke *keys);

#endif // __RMTDOS_CLIENT_NETWORK_H
