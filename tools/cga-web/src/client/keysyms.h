/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __RMTDOS_CLIENT_KEYSYMS_H
#define __RMTDOS_CLIENT_KEYSYMS_H

// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html

// Raw scan-codes
#define SCAN_ESC 1
#define SCAN_1 2
#define SCAN_2 3
#define SCAN_3 4
#define SCAN_4 5
#define SCAN_5 6
#define SCAN_6 7
#define SCAN_7 8
#define SCAN_8 9
#define SCAN_9 10
#define SCAN_0 11
#define SCAN_DASH 12
#define SCAN_EQ 13
#define SCAN_BS 14

#define SCAN_TAB 15
#define SCAN_Q 16
#define SCAN_W 17
#define SCAN_E 18
#define SCAN_R 19
#define SCAN_T 20
#define SCAN_Y 21
#define SCAN_U 22
#define SCAN_I 23
#define SCAN_O 24
#define SCAN_P 25
#define SCAN_LBRACE 26 /* [ */
#define SCAN_RBRACE 27 /* ] */
#define SCAN_RETURN 28 /* Return */

#define SCAN_LCTRL 29 /* left control */
#define SCAN_A 30
#define SCAN_S 31
#define SCAN_D 32
#define SCAN_F 33
#define SCAN_G 34
#define SCAN_H 35
#define SCAN_J 36
#define SCAN_K 37
#define SCAN_L 38
#define SCAN_SEMICOLON 39
#define SCAN_QUOTE 40
#define SCAN_TILDE 41
#define SCAN_LSHIFT 42
#define SCAN_BSLASH 43

#define SCAN_Z 44
#define SCAN_X 45
#define SCAN_C 46
#define SCAN_V 47
#define SCAN_B 48
#define SCAN_N 49
#define SCAN_M 50
#define SCAN_COMMA 51
#define SCAN_PERIOD 52
#define SCAN_FSLASH 53
#define SCAN_RSHIFT 54
#define SCAN_MUL 55  /* keypad '*' */
#define SCAN_LALT 56 /* left alt */
#define SCAN_SPACE 57
#define SCAN_CAPS_LOCK 58

#define SCAN_F1 0x3b
#define SCAN_F2 0x3c
#define SCAN_F3 0x3d
#define SCAN_F4 0x3e
#define SCAN_F5 0x3f
#define SCAN_F6 0x40
#define SCAN_F7 0x41
#define SCAN_F8 0x42
#define SCAN_F9 0x43
#define SCAN_F10 0x44
#define SCAN_F11 0x85
#define SCAN_F12 0x86

#define SCAN_KEYPAD_7 0x47
#define SCAN_KEYPAD_8 0x48
#define SCAN_KEYPAD_9 0x49
#define SCAN_KEYPAD_MINUS 0x4a
#define SCAN_KEYPAD_4 0x4b
#define SCAN_KEYPAD_5 0x4c
#define SCAN_KEYPAD_6 0x4d
#define SCAN_KEYPAD_PLUS 0x4e
#define SCAN_KEYPAD_1 0x4f
#define SCAN_KEYPAD_2 0x50
#define SCAN_KEYPAD_3 0x51
#define SCAN_KEYPAD_0 0x52
#define SCAN_KEYPAD_PERIOD 0x53

#define SCAN_HOME SCAN_KEYPAD_7
#define SCAN_UP SCAN_KEYPAD_8
#define SCAN_PAGE_UP SCAN_KEYPAD_9
#define SCAN_LEFT SCAN_KEYPAD_4
#define SCAN_RIGHT SCAN_KEYPAD_6
#define SCAN_END SCAN_KEYPAD_1
#define SCAN_DOWN SCAN_KEYPAD_2
#define SCAN_PAGE_DOWN SCAN_KEYPAD_3
#define SCAN_INSERT SCAN_KEYPAD_0
#define SCAN_DELETE SCAN_KEYPAD_PERIOD

#endif // __RMTDOS_CLIENT_KEYSYMS_H
