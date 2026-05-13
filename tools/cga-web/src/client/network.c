/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/random.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client/network.h"

// Raw packet send/recv code inspired by
// https://gist.github.com/lethean/5fb0f493a1968939f2f7

static const uint8_t g_broadcast_addr[ETH_ALEN] = {0xff, 0xff, 0xff,
                                                   0xff, 0xff, 0xff};

uint32_t create_session_id() {
  uint32_t session_id = 0;
  getrandom(&session_id, sizeof(session_id), 0);
  return session_id;
}

// Returns error-like values: 0 on success, <0 on error, >0 (socket) on
// success.
int create_socket(struct RawSocket *result, const char *if_name,
                  uint16_t ethertype) {
  memset(result, 0, sizeof(*result));
  result->sock_fd = -1;

  result->ethertype = ethertype;
  result->session_id = create_session_id();
  result->if_name = if_name;

  int r = socket(AF_PACKET, SOCK_RAW, htons(ethertype));
  if (r < 0) {
    perror("socket()");
    goto fail;
  }
  result->sock_fd = r;

  // Get the index number and MAC address of the ethernet interface.
  struct ifreq ifr = {0};
  strncpy(ifr.ifr_name, if_name, IFNAMSIZ - 1);

  if (0 > (r = ioctl(result->sock_fd, SIOCGIFINDEX, &ifr))) {
    perror("SIOCGIFINDEX");
    goto fail;
  }
  result->if_index = ifr.ifr_ifindex;

  if (0 > (r = ioctl(result->sock_fd, SIOCGIFHWADDR, &ifr))) {
    perror("SIOCGIFHWADDR");
    goto fail;
  }
  memcpy(result->if_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

  // Bind to device.
  if (0 > (r = setsockopt(result->sock_fd, SOL_SOCKET, SO_BINDTODEVICE, if_name,
                          IFNAMSIZ - 1))) {
    perror("SO_BINDTODEVICE");
    goto fail;
  }

  return result->sock_fd;

fail:
  if (result->sock_fd >= 0) {
    close(result->sock_fd);
    result->sock_fd = -1;
  }

  return r;
}

void close_socket(struct RawSocket *sock) {
  if (sock->sock_fd >= 0) {
    close(sock->sock_fd);
    sock->sock_fd = -1;
  }
}

int send_packet(struct RawSocket *sock, const uint8_t *dest_mac_addr,
                enum PKT_TYPE pkt_type, const void *payload,
                size_t payload_len) {
  assert(sock);
  assert(payload_len <= MAX_PAYLOAD_LENGTH);

  const uint8_t *dest = dest_mac_addr ? dest_mac_addr : g_broadcast_addr;
  const size_t send_len =
      sizeof(struct ether_header) + sizeof(struct ProtocolHeader) + payload_len;
  uint8_t *buffer = malloc(send_len);

  // Configure ethernet header.
  struct ether_header *eh = (struct ether_header *)buffer;
  memcpy(eh->ether_shost, sock->if_addr, ETH_ALEN);
  memcpy(eh->ether_dhost, dest, ETH_ALEN);
  eh->ether_type = htons(sock->ethertype);

  struct ProtocolHeader *ph = (struct ProtocolHeader *)(eh + 1);
  ph->signature = htonl(PACKET_SIGNATURE);
  ph->session_id = htonl(sock->session_id);
  ph->payload_len = htons(payload_len);
  ph->pkt_type = htons(pkt_type);

  // Place our payload immediately after the headers.
  if (payload) {
    memcpy(ph + 1, payload, payload_len);
  }

  // Where to send the packet.
  struct sockaddr_ll sock_addr = {0};
  sock_addr.sll_ifindex = sock->if_index;
  sock_addr.sll_halen = ETH_ALEN;
  memcpy(sock_addr.sll_addr, dest, ETH_ALEN);

  int r = sendto(sock->sock_fd, buffer, send_len, 0,
                 (struct sockaddr *)&sock_addr, sizeof(sock_addr));
  if (r < 0) {
    perror("sendto()");
  }

  free(buffer);

  return r;
}

int send_ping(struct RawSocket *sock, const uint8_t *dest_mac_addr) {
  // Actual ping payload should not matter.
  const char payload[4] = {'P', 'I', 'N', 'G'};

  return send_packet(sock, dest_mac_addr, V1_PING, payload, sizeof(payload));
}

int send_status_req(struct RawSocket *sock, const uint8_t *dest_mac_addr) {
  return send_packet(sock, dest_mac_addr, V1_STATUS_REQ, NULL, 0);
}

int send_session_start(struct RawSocket *sock, const uint8_t *dest_mac_addr) {
  return send_packet(sock, dest_mac_addr, V1_SESSION_START, NULL, 0);
}

int send_keystrokes(struct RawSocket *sock, const uint8_t *dest_mac_addr,
                    size_t count, const struct Keystroke *keys) {
  return send_packet(sock, dest_mac_addr, V1_INJECT_KEYSTROKE, keys,
                     count * sizeof(struct Keystroke));
}
