# SPDX-License-Identifier: MIT
# Author:  Giovanni Santini
# Mail:    giovanni.santini@proton.me
# Github:  @San7o

#
# Compile flags
#
CFLAGS=-Wall -Werror -Wpedantic -O2 -Wno-unused-function -std=c99
LDFLAGS=
CC=gcc

#
# Project files
#
OUT_NAME=example
OBJ=example.o

MICRO_TESTS_LINKER_SCRIPT=tests/micro-tests.ld
TEST_OBJ=tests/tests.o
TEST_OUT_NAME=test

#
# Commands
#
all: $(OUT_NAME)

run: $(OUT_NAME)
	chmod +x $(OUT_NAME)
	./$(OUT_NAME)

check: $(TEST_OUT_NAME)
	chmod +x $(TEST_OUT_NAME)
	./$(TEST_OUT_NAME) --multithreaded --threads $(shell nproc) --quiet --no-banner

clean:
	rm -f $(OBJ) $(TEST_OBJ)

distclean:
	rm -f $(OUT_NAME) $(TEST_OUT_NAME)

$(OUT_NAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(CLAGS) -o $(OUT_NAME)

$(TEST_OUT_NAME): $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(LDFLAGS) $(CFLAGS) -o $(TEST_OUT_NAME)	-Wl,-T,${MICRO_TESTS_LINKER_SCRIPT}

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
