/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef RMTDOS_FILE_COMMANDER_UI_COMMANDER_H
#define RMTDOS_FILE_COMMANDER_UI_COMMANDER_H

#include <stdint.h>

struct CommanderConfig {
  const char *if_name;
  uint16_t ethertype;
  int host_addr_set;
  uint8_t host_addr[6];
};

int commander_run(const struct CommanderConfig *config);

#endif

