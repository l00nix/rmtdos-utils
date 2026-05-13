/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef RMTDOS_SERVER_INT2F_H
#define RMTDOS_SERVER_INT2F_H

#include "lib16/x86.h"

// MS-DOS "Multiplex Interrupt".  Used to communicate between TSR and another
// instance that wants to check for installation or request removal of the TSR.

// https://stanislavs.org/helppc/int_2f-0.html
// Value chosen arbitrarily, but must match value in 'server/int2f.s'
#define MULTIPLEX_ID 0xfb

#define MULTIPLEX_MAGIC_AX 0xfb00
#define MULTIPLEX_MAGIC_BX 0x5a98

// Our installed interrupt handler ('int2f_isr') will check AX and BX, and if
// they match, will then call 'int2f_handler'.  Our handler should clear BX
// as a sign that we're installed.  Commands to be executed (in DX):

#define MULTIPLEX_CMD_INSTALL_CHECK 0
#define MULTIPLEX_CMD_UNINSTALL 1

// Exposed so that main() can memset() the stack before installing the ISR,
// so that we can observe stack usage via DEBUG.COM.
extern uint8_t *int2f_stack_top, *int2f_stack_bottom;

// Must be implemented in C, elsewhere.
extern void int2f_handler(struct CpuRegs *regs);

// Implemented in 'server/int2f.s'
extern void int2f_isr();

#endif // RMTDOS_SERVER_INT2F_H
