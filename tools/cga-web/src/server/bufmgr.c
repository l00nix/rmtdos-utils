/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib16/x86.h"
#include "server/bufmgr.h"
#include "server/util.h"

static size_t g_buffer_count = 0;

static struct Buffer *g_buffers = NULL;

// Root pointer of the free list.
static struct Buffer *g_free_list = NULL;

// Pointer to the next 'ready' buffer.
static struct Buffer *g_ready_list = NULL;

#if DEBUG
void buffer_debug_dump(FILE *fp) {
  int i, j;
  uint16_t saved_flags = x86_cli();

  fprintf(fp, "free_list = %p, ready_list = %p\n", g_free_list, g_ready_list);

  for (i = 0; i < g_buffer_count; i++) {
    const struct Buffer *b = g_buffers + i;
    fprintf(fp, "[%04x] %d %04x %4d  ", b, b->state, b->next, b->bytes);
    for (j = 0; j < 20; j++) {
      if ((j & 1) == 0) {
        fputc(' ', fp);
      }
      fprintf(fp, "%02x", b->data[j]);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");

  x86_sti(saved_flags);
}
#endif // DEBUG

void buffer_init(size_t count) {
  size_t i;

  g_buffers = (struct Buffer *)malloc(sizeof(struct Buffer) * count);
  if (!g_buffers) {
    fprintf(stderr, "Failed to allocate %d buffers, aborting.\n", count);
    exit(EXIT_FAILURE);
  }

  memset(g_buffers, 0, sizeof(struct Buffer) * count);
  g_buffer_count = count;
  g_free_list = g_buffers;
  g_ready_list = NULL;

  for (i = 0; i < g_buffer_count - 1; i++) {
    g_buffers[i].next = g_buffers + i + 1;
  }
}

// WARNING: Called from inside an ISR.  It is NOT safe to call any DOS, BIOS
// or heap functions.  Assumed called with interrupts disabled.
void *buffer_acquire(size_t bytes) {
  struct Buffer *node;

  if (!g_free_list || (bytes > BUFFER_MAX_SIZE)) {
    return NULL;
  }

  node = g_free_list;
  g_free_list = g_free_list->next;

  node->bytes = bytes;
  node->state = BUFFER_PENDING;

  // Clear the next pointer out of the buffer, so that it is pristine.
  node->next = NULL;

  return node->data;
}

// WARNING: Called from inside an ISR.  It is NOT safe to call any DOS, BIOS
// or heap functions.  Assumed called with interrupts disabled.
void buffer_mark_ready(void *data) {
  // Step #1, determine which buffer it is.
  struct Buffer *buf =
      (struct Buffer *)((uint8_t *)data - offsetof(struct Buffer, data));

  // We can only hope that the packet driver passed us good data.
  // If the address is wrong, or the "buf->state != BUFFER_PENDING", then
  // we have an error, but no easy way to handle it.
  buf->state = BUFFER_READY;
  buf->next = g_ready_list;
  g_ready_list = buf;
}

struct Buffer *buffer_get_ready() {
  struct Buffer *retval = NULL;
  uint16_t saved_flags = x86_cli();

  if (g_ready_list) {
    retval = g_ready_list;
    g_ready_list = g_ready_list->next;
    retval->state = BUFFER_USER;
    retval->next = NULL;
  }

  x86_sti(saved_flags);

  return retval;
}

void buffer_release(struct Buffer *buffer) {
  uint16_t saved_flags;

  memset(buffer->data, 0, sizeof(buffer->data));
  saved_flags = x86_cli();

  buffer->state = BUFFER_FREE;
  buffer->bytes = 0;
  buffer->next = g_free_list;
  g_free_list = buffer;

  x86_sti(saved_flags);
}

#if 0
void test_bufmgr() {
  int i;
  void *bufs[g_buffer_count + 2];

  memset(bufs, 0, sizeof(bufs));

  buffer_init();
  buffer_debug_dump();

  // Leave two buffers left in the 'free' state.
  for (i = 0; i < g_buffer_count - 2; i++) {
    bufs[i] = buffer_acquire(5 + (i % 4));
    printf("buffer_acquire() = %p [%d]\n", bufs[i], i);
  }

  buffer_debug_dump();

  strcpy(bufs[1], "hello world");
  buffer_mark_ready(bufs[1]);
  strcpy(bufs[3], "foobar");
  buffer_mark_ready(bufs[3]);
  buffer_debug_dump();

  do {
    struct Buffer *buf = buffer_get_ready();
    printf("\nbuffer_get_ready() = %p, %d: ", buf, buf->bytes);
    hex_dump(stdout, buf->data, 8);

    buffer_release(buf);
  } while (0);

  buffer_debug_dump();
}
#endif
