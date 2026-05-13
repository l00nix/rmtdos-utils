# SPDX-License-Identifier: GPL-2.0-or-later

.PHONY: all cga-web file-commander clean format

all: cga-web file-commander

cga-web:
	$(MAKE) -C tools/cga-web

file-commander:
	$(MAKE) -C tools/file-commander

clean:
	$(MAKE) -C tools/cga-web clean
	$(MAKE) -C tools/file-commander clean

format:
	$(MAKE) -C tools/cga-web format
	$(MAKE) -C tools/file-commander format
