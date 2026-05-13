# SPDX-License-Identifier: GPL-2.0-or-later

OUTDIR := out
UNIFIED_BUILD := $(OUTDIR)/rmtdos-utils-build
UTILS_BIN := $(OUTDIR)/rmtdos-utils

CC ?= cc
PKG_CONFIG ?= pkg-config
NCURSESW_FLAGS := $(shell $(PKG_CONFIG) ncursesw --cflags --libs)

CGA_CLIENT_SRC := \
	tools/cga-web/src/client/curses.c \
	tools/cga-web/src/client/file_transfer.c \
	tools/cga-web/src/client/hostlist.c \
	tools/cga-web/src/client/keyboard.c \
	tools/cga-web/src/client/main.c \
	tools/cga-web/src/client/network.c \
	tools/cga-web/src/client/util.c \
	tools/cga-web/src/client/web.c

CGA_CLIENT_OBJ := \
	$(patsubst tools/cga-web/src/client/%.c,$(UNIFIED_BUILD)/cga/%.o,$(CGA_CLIENT_SRC))

FC_SRC := \
	tools/file-commander/src/net/file_transfer.c \
	tools/file-commander/src/net/hostlist.c \
	tools/file-commander/src/net/raw_socket.c \
	tools/file-commander/src/net/remote_dir.c \
	tools/file-commander/src/ui/commander.c

FC_OBJ := \
	$(patsubst tools/file-commander/src/%.c,$(UNIFIED_BUILD)/fc/%.o,$(FC_SRC))

FC_RENAMES := \
	-Dcommander_run=fc_commander_run \
	-Dcreate_session_id=fc_create_session_id \
	-Dcreate_socket=fc_create_socket \
	-Dclose_socket=fc_close_socket \
	-Dsend_packet=fc_send_packet \
	-Dsend_status_req=fc_send_status_req \
	-Dsend_session_start=fc_send_session_start \
	-Dhostlist_create=fc_hostlist_create \
	-Dhostlist_destroy=fc_hostlist_destroy \
	-Dhostlist_find_by_mac=fc_hostlist_find_by_mac \
	-Dhostlist_find_by_index=fc_hostlist_find_by_index \
	-Dhostlist_iter=fc_hostlist_iter \
	-Dhostlist_register=fc_hostlist_register \
	-Dfmt_mac_addr=fc_fmt_mac_addr \
	-Dfile_transfer_put=fc_file_transfer_put \
	-Dfile_transfer_get=fc_file_transfer_get \
	-Dfile_remote_mkdir=fc_file_remote_mkdir \
	-Dfile_remote_delete=fc_file_remote_delete \
	-Dfile_remote_rename=fc_file_remote_rename \
	-Dfile_remote_copy=fc_file_remote_copy \
	-Dremote_dir_fetch=fc_remote_dir_fetch

.PHONY: all cga-web file-commander clean format

all: $(UTILS_BIN) cga-web file-commander

$(OUTDIR) $(UNIFIED_BUILD):
	mkdir -p $@

$(UTILS_BIN): $(UNIFIED_BUILD)/rmtdos-utils.o $(CGA_CLIENT_OBJ) $(FC_OBJ) | $(OUTDIR)
	$(CC) -std=c99 -Wall -Wextra -g -o $@ $^ $(NCURSESW_FLAGS)

$(UNIFIED_BUILD)/rmtdos-utils.o: src/rmtdos-utils.c | $(UNIFIED_BUILD)
	$(CC) -std=c99 -Wall -Wextra -g -Itools/cga-web/src \
	      $(shell $(PKG_CONFIG) ncursesw --cflags) -c -o $@ $<

$(UNIFIED_BUILD)/cga/%.o: tools/cga-web/src/client/%.c | $(UNIFIED_BUILD)
	@mkdir -p $(dir $@)
	$(CC) -std=c99 -Wall -Itools/cga-web/src -DRMTDOS_CGA_WEB_NO_MAIN \
	      -ggdb $(shell $(PKG_CONFIG) ncursesw --cflags) -c -o $@ $<

$(UNIFIED_BUILD)/fc/%.o: tools/file-commander/src/%.c | $(UNIFIED_BUILD)
	@mkdir -p $(dir $@)
	$(CC) -std=c99 -Wall -Wextra -Wpedantic -g -Itools/file-commander/src \
	      $(FC_RENAMES) $(shell $(PKG_CONFIG) ncursesw --cflags) -c -o $@ $<

cga-web:
	$(MAKE) -C tools/cga-web

file-commander:
	$(MAKE) -C tools/file-commander

clean:
	rm -rf $(OUTDIR)
	$(MAKE) -C tools/cga-web clean
	$(MAKE) -C tools/file-commander clean

format:
	clang-format -i src/*.c
	$(MAKE) -C tools/cga-web format
	$(MAKE) -C tools/file-commander format
