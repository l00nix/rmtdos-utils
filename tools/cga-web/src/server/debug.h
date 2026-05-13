/*
 * Copyright 2022 Dennis Jenkins <dennis.jenkins.75@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// Debugging tools.

#include <stdio.h>

#include "lib16/types.h"

void hex_dump(FILE *fp, const void *buffer, size_t bytes);

void log_checkpoint(const char *file, int line);

#define CP() log_checkpoint(__FILE__, __LINE__)
