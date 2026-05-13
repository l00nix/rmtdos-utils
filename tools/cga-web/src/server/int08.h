/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef RMTDOS_SERVER_INT08_H
#define RMTDOS_SERVER_INT08_H

#include "lib16/x86.h"

// Exposed so that main() can memset() the stack before installing the ISR,
// so that we can observe stack usage via DEBUG.COM.
extern uint8_t *int08_stack_top, *int08_stack_bottom;

// Must be implemented in C, elsewhere.
extern void int08_handler();

// Implemented in 'server/int08.s'
extern void int08_isr();

#endif //  RMTDOS_SERVER_INT08_H
