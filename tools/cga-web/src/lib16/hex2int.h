/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __RMTDOS_LIB16_HEX2INT_H
#define __RMTDOS_LIB16_HEX2INT_H

#include "lib16/types.h"

/* Assumes input is raw hex string ("a73d"), with no leading "0x".
   Stops parsing on first non-hex character.
*/
extern uint16_t hex_to_uint16(const char *str);

#endif /* __RMTDOS_LIB16_HEX2INT_H */
