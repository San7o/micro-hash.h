# SPDX-License-Identifier: MIT

## --- Settings ---

CFLAGS=-Wall -Werror -Wpedantic -O2 -Wno-unused-function -std=c99
LDFLAGS=
CC=gcc

OUT_NAME=example
OBJ=example.o

MICRO_TESTS_LINKER_SCRIPT=tests/micro-tests.ld
TEST_OBJ=tests/tests.o
TEST_OUT_NAME=test

## --- Commands ---

# --- Targets ---

all: $(OUT_NAME)

run: $(OUT_NAME)
	chmod +x $(OUT_NAME)
	./$(OUT_NAME)

check: $(TEST_OUT_NAME)
	chmod +x $(TEST_OUT_NAME)
	./$(TEST_OUT_NAME) --multithreaded --threads $(shell nproc) --quiet --no-banner

$(OUT_NAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(CLAGS) -o $(OUT_NAME)

$(TEST_OUT_NAME): $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(LDFLAGS) $(CLAGS) -o $(TEST_OUT_NAME)	-Wl,-T,${MICRO_TESTS_LINKER_SCRIPT}

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ) $(TEST_OBJ) 2>/dev/null || :

distclean:
	rm $(OUT_NAME) $(TEST_OUT_NAME) 2>/dev/null || :
