/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __RMTDOS_LIB16_VGA_H
#define __RMTDOS_LIB16_VGA_H

#include "lib16/types.h"

#define VGA_ROWS 25
#define VGA_COLS 80
#define VGA_WORD 2
#define VGA_SIZE (VGA_ROWS * VGA_COLS * VGA_WORD)

extern void vga_mode_80x25();
extern void vga_mode_80x43();
extern void vga_mode_80x50();

// Copy 'words' count of WORDS (16-bit ints) from VGA Text Mode frame buffer,
// starting at offset from [$b800:0000] into dest.
extern void vga_copy_from_frame_buffer(void *dest, uint16_t offset,
                                       uint16_t words);

extern void vga_gotoxy(int x, int y);

extern void vga_write_str(int x, int y, uint8_t attr, const char *str);

extern int vga_printf(int x, int y, uint8_t attr, const char *fmt, ...);

// Erase all screen text from "y1 <= y < y2".
extern void vga_clear_rows(int y1, int y2);

struct VgaState {
  uint8_t video_mode;  // int $10, ah=$0f, al value
  uint8_t active_page; // int $10, ah=$0f, bh value
  uint8_t text_rows;   // (no direct query for this)
  uint8_t text_cols;   // int $10, ah=$0f, ah value
  uint8_t cursor_row;  // int $10, ah=$03, dh value
  uint8_t cursor_col;  // int $10, ah=$03, dl value
};

extern void vga_read_state(struct VgaState *state);

extern void vga_disable_blink();

#endif // __RMTDOS_LIB16_VGA_H
