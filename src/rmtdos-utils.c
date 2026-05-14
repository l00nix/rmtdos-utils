/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <errno.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <locale.h>
#include <ncurses.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "client/hostlist.h"
#include "client/network.h"
#include "client/util.h"
#include "common/protocol.h"

#define RMTDOS_UTILS_VERSION "rmtdos-utils v0.1.0"

enum LaunchMode {
  LAUNCH_NONE = 0,
  LAUNCH_SHELL,
  LAUNCH_FILE_COMMANDER,
};

struct CommanderConfig {
  const char *if_name;
  uint16_t ethertype;
  int host_addr_set;
  uint8_t host_addr[ETH_ALEN];
};

int fc_commander_run(const struct CommanderConfig *config);
int rmtdos_cga_web_client_main(int argc, char **argv);

static const char *DEFAULT_ETH_DEV = "eth0";
static const struct timeval PROBE_INTERVAL = {
    .tv_sec = 2,
    .tv_usec = 0,
};

static void usage(FILE *out, const char *argv0) {
  fprintf(out, "Usage: %s [-i IFACE] [-e ETHERTYPE] [-w|-W ADDR[:PORT]]\n",
          argv0);
  fprintf(out, "\n");
  fprintf(out, "Options:\n");
  fprintf(out, "  -i IFACE      Linux network interface, default %s\n",
          DEFAULT_ETH_DEV);
  fprintf(out, "  -e ETHERTYPE  Hex EtherType, default %04x\n",
          ETHERTYPE_RMTDOS);
  fprintf(out, "  -w            Enable shell-mode CGA web view on 127.0.0.1:8080\n");
  fprintf(out, "  -W ADDR[:PORT] Enable shell-mode CGA web view on ADDR[:PORT]\n");
  fprintf(out, "  -h            Show this help\n");
  fprintf(out, "  -v            Show version and exit\n");
}

static int parse_ethertype(const char *text, uint16_t *out) {
  char *end = NULL;
  unsigned long value;

  errno = 0;
  value = strtoul(text, &end, 16);
  if (errno || !end || *end || value > 0xffff) {
    return -1;
  }

  *out = (uint16_t)value;
  return 0;
}

static void mac_to_text(char *out, size_t out_len, const uint8_t *mac) {
  snprintf(out, out_len, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1],
           mac[2], mac[3], mac[4], mac[5]);
}

static int socket_has_data(int fd, int timeout_ms) {
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;
  return select(fd + 1, &fds, NULL, NULL, &tv) > 0;
}

static void process_incoming_status(struct RawSocket *sock) {
  uint8_t buf[ETH_FRAME_LEN];
  ssize_t received;
  const struct ether_header *eh;
  const struct ProtocolHeader *ph;

  received = recv(sock->sock_fd, buf, sizeof(buf), 0);
  if (received < (ssize_t)(sizeof(*eh) + sizeof(*ph))) {
    return;
  }

  eh = (const struct ether_header *)buf;
  ph = (const struct ProtocolHeader *)(eh + 1);

  if (memcmp(eh->ether_dhost, sock->if_addr, ETH_ALEN) ||
      ntohl(ph->signature) != PACKET_SIGNATURE ||
      ntohl(ph->session_id) != sock->session_id) {
    return;
  }

  if (ntohs(ph->pkt_type) == V1_STATUS_RESP) {
    hostlist_register(buf, received);
  }
}

static void draw_host_selector(const struct RawSocket *sock) {
  int rows;
  int cols;
  int y = 1;
  int iter = 0;
  struct RemoteHost *host;
  struct timeval now;

  erase();
  getmaxyx(stdscr, rows, cols);
  box(stdscr, 0, 0);
  mvprintw(0, 2, " rmtdos-utils host selector ");
  mvprintw(y++, 2, "%s", RMTDOS_UTILS_VERSION);
  mvprintw(y++, 2, "Interface: %s  EtherType: %04x", sock->if_name,
           sock->ethertype);
  y++;

  attron(COLOR_PAIR(2) | A_BOLD);
  mvprintw(y++, 2, "Id  MAC address        Mode   Size    Last seen");
  attroff(COLOR_PAIR(2) | A_BOLD);

  gettimeofday(&now, NULL);
  while ((host = hostlist_iter(&iter)) && y < rows - 3) {
    struct timeval diff;
    char mac[MAC_ADDR_FMT_LEN];

    timersub(&now, &host->tv_last_resp, &diff);
    mvprintw(y++, 2, "%2d  %s  %4d   %3dx%-3d %ld.%03lds", host->index,
             fmt_mac_addr(mac, sizeof(mac), host->if_addr),
             host->status.video_mode, host->status.text_cols,
             host->status.text_rows, diff.tv_sec, diff.tv_usec / 1000);
  }

  mvprintw(rows - 2, 2, "Press 0-9 to select a host. q/Esc/Ctrl-] exits.");
  refresh();
  (void)cols;
}

static struct RemoteHost *select_host(struct RawSocket *sock) {
  struct timeval last_probe = {0};

  while (1) {
    struct timeval now;
    struct timeval diff;
    int ch;

    gettimeofday(&now, NULL);
    timersub(&now, &last_probe, &diff);
    if (!timerisset(&last_probe) || timercmp(&diff, &PROBE_INTERVAL, >=)) {
      send_status_req(sock, NULL);
      last_probe = now;
    }

    while (socket_has_data(sock->sock_fd, 20)) {
      process_incoming_status(sock);
    }

    draw_host_selector(sock);
    ch = getch();
    if (ch == ERR) {
      continue;
    }
    if (ch == 'q' || ch == 'Q' || ch == 27 || ch == 29) {
      return NULL;
    }
    if (ch >= '0' && ch <= '9') {
      struct RemoteHost *host = hostlist_find_by_index(ch - '0');
      if (host) {
        return host;
      }
    }
  }
}

static enum LaunchMode select_mode(const struct RemoteHost *host) {
  int rows;
  int cols;
  char mac[MAC_ADDR_FMT_LEN];

  nodelay(stdscr, FALSE);
  while (1) {
    int ch;

    erase();
    getmaxyx(stdscr, rows, cols);
    (void)cols;
    box(stdscr, 0, 0);
    mvprintw(0, 2, " rmtdos-utils mode selector ");
    mvprintw(2, 2, "Host: %s", fmt_mac_addr(mac, sizeof(mac), host->if_addr));
    mvprintw(4, 4, "s  Remote shell");
    mvprintw(5, 4, "f  File commander");
    mvprintw(rows - 2, 2, "Choose mode. q/Esc/Ctrl-] exits.");
    refresh();

    ch = getch();
    if (ch == 's' || ch == 'S') {
      return LAUNCH_SHELL;
    }
    if (ch == 'f' || ch == 'F') {
      return LAUNCH_FILE_COMMANDER;
    }
    if (ch == 'q' || ch == 'Q' || ch == 27 || ch == 29) {
      return LAUNCH_NONE;
    }
  }
}

static int run_shell_mode(const char *if_name, uint16_t ethertype,
                          const uint8_t *host_addr, int web_enabled,
                          const char *web_listen) {
  char ethertype_text[8];
  char mac_text[MAC_ADDR_FMT_LEN];
  char *argv[12];
  int argc = 0;

  snprintf(ethertype_text, sizeof(ethertype_text), "%04x", ethertype);
  mac_to_text(mac_text, sizeof(mac_text), host_addr);

  argv[argc++] = "rmtdos-cga-web-client";
  argv[argc++] = "-i";
  argv[argc++] = (char *)if_name;
  argv[argc++] = "-e";
  argv[argc++] = ethertype_text;
  argv[argc++] = "-d";
  argv[argc++] = mac_text;
  if (web_listen) {
    argv[argc++] = "-W";
    argv[argc++] = (char *)web_listen;
  } else if (web_enabled) {
    argv[argc++] = "-w";
  }
  argv[argc] = NULL;

  optind = 1;
  return rmtdos_cga_web_client_main(argc, argv);
}

static int run_file_commander_mode(const char *if_name, uint16_t ethertype,
                                   const uint8_t *host_addr) {
  struct CommanderConfig config;

  memset(&config, 0, sizeof(config));
  config.if_name = if_name;
  config.ethertype = ethertype;
  config.host_addr_set = 1;
  memcpy(config.host_addr, host_addr, sizeof(config.host_addr));

  return fc_commander_run(&config);
}

int main(int argc, char **argv) {
  const char *if_name = DEFAULT_ETH_DEV;
  const char *web_listen = NULL;
  uint16_t ethertype = ETHERTYPE_RMTDOS;
  int web_enabled = 0;
  int opt;

  while ((opt = getopt(argc, argv, "e:hi:vwW:")) != -1) {
    switch (opt) {
      case 'e':
        if (parse_ethertype(optarg, &ethertype)) {
          fprintf(stderr, "invalid EtherType: %s\n", optarg);
          return 2;
        }
        break;
      case 'h':
        usage(stdout, argv[0]);
        return 0;
      case 'i':
        if_name = optarg;
        break;
      case 'v':
        puts(RMTDOS_UTILS_VERSION);
        return 0;
      case 'w':
        web_enabled = 1;
        break;
      case 'W':
        web_enabled = 1;
        web_listen = optarg;
        break;
      default:
        usage(stderr, argv[0]);
        return 2;
    }
  }

  if (optind < argc) {
    usage(stderr, argv[0]);
    return 2;
  }

  setlocale(LC_ALL, "");

  while (1) {
    struct RawSocket sock = {0};
    struct RemoteHost *host;
    uint8_t host_addr[ETH_ALEN];
    enum LaunchMode mode;
    int rc = 0;

    if (create_socket(&sock, if_name, ethertype) < 0) {
      return 1;
    }
    hostlist_create();

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_CYAN, -1);

    host = select_host(&sock);
    if (!host) {
      endwin();
      hostlist_destroy();
      close_socket(&sock);
      return 0;
    }
    memcpy(host_addr, host->if_addr, sizeof(host_addr));

    mode = select_mode(host);
    endwin();
    hostlist_destroy();
    close_socket(&sock);

    if (mode == LAUNCH_NONE) {
      return 0;
    }
    if (mode == LAUNCH_SHELL) {
      rc = run_shell_mode(if_name, ethertype, host_addr, web_enabled,
                          web_listen);
    } else if (mode == LAUNCH_FILE_COMMANDER) {
      rc = run_file_commander_mode(if_name, ethertype, host_addr);
    }

    if (rc) {
      return rc;
    }
  }
}
