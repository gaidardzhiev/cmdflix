# Copyright (C) 2025 Alexander Parvanov and Ivan Gaydardzhiev; Licensed under the GPL-3.0-only

CC=gcc
BIN=cmdflix

all: $(BIN)

$(BIN): %: %.c
	$(CC) -o $@ $<

clean:
	rm $(BIN)

install:
	cp $(BIN) /usr/bin/$(BIN)

strip:
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag $(BIN)
