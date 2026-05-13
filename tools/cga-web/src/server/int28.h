/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef RMTDOS_SERVER_INT28_H
#define RMTDOS_SERVER_INT28_H

#include "lib16/x86.h"
#include "server/config.h"

#if HAS_INT28

// Exposed so that main() can memset() the stack before installing the ISR,
// so that we can observe stack usage via DEBUG.COM.
extern uint8_t *int28_stack_top, *int28_stack_bottom;

// Must be implemented in C, elsewhere.
extern void int28_handler();

// Implemented in 'server/int28.s'
extern void int28_isr();

#endif

#endif //  RMTDOS_SERVER_INT28_H
