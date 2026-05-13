/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// Buffer Manager for receiving packets from the packet driver.
//
// Implements a simple static table of entries.  Each buffer can hold an MTU
// (1500 bytes) of raw packet data, plus a few bytes of overhead.  We cannot
// fit very many of these into RAM at once, so we'll optimize for simplicity
// and not use a complicated data structure for managing an arbitrary amount
// of buffers.

#ifndef __RMTDOS_SERVER_BUFMGR_H
#define __RMTDOS_SERVER_BUFMGR_H

#include <stdio.h>
#include <sys/types.h>

#include "lib16/types.h"
#include "server/config.h"

// Max size of each buffer's payload area.  Value comes from Ethernet 802.3
// standard, so no need to make it configurable.
#define BUFFER_MAX_SIZE 1504

enum BufferState {
  BUFFER_FREE = 0,    // Buffer available for packet driver.
  BUFFER_PENDING = 1, // Buffer held by packet driver.
  BUFFER_READY = 2,   // Buffer returned from driver, waiting for consumer.
  BUFFER_USER = 3,    // Buffer held by consumer.
};

struct Buffer {
  enum BufferState state;

  // Count of bytes copied into the buffer.
  // 0 when buffer is free.
  size_t bytes;

  // Next buffer of the same state.
  struct Buffer *next;

  // Raw packet data.
  uint8_t data[BUFFER_MAX_SIZE];
};

// Initialize the buffer manager (create initial free list).
extern void buffer_init(size_t count);

// Called by the packet driver (while servicing an interrupt) to request a
// buffer.  Returns raw data pointer (payload.data) or NULL.
extern void *buffer_acquire(size_t bytes);

// Called by the packet driver (while servicing an interrupt) to indicate that
// the driver is done writing to a buffer.
extern void buffer_mark_ready(void *data);

// Called by our protocol handler to get a 'ready' buffer (one ready for us
// to consume).  Returns NULL if none are ready.
extern struct Buffer *buffer_get_ready();

// Called by our protocol handler when it is done with a buffer and wants
// to return it to the 'free' list.
extern void buffer_release(struct Buffer *buffer);

#if DEBUG
extern void buffer_debug_dump(FILE *fp);
#endif

#endif /* __RMTDOS_SERVER_BUFMGR_H */
