/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 'bcc' does not seem to have a 'stdint.h' that I can include. */

#ifndef __RMTDOS_LIB16_TYPES_H__
#define __RMTDOS_LIB16_TYPES_H__

#ifdef __GNUC__
#include <stdint.h>

#else
/* BCC does not have stdint.h */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
#endif

#endif /* __RMTDOS_LIB16_TYPES_H__ */
