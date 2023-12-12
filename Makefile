CCACHE = ccache
CC = clang

CFLAGS = -std=c99 -Wall -Wextra -g

INCLUDE_DIRS = -Isrc -Ivendor/stb

ALL_SRCS = $(wildcard src/*/*.c)
TEST_SRCS = $(wildcard src/*/*.test.c)

ALL_OBJS = $(patsubst src/%.c,build/obj/%.o,$(ALL_SRCS))
TEST_OBJS = $(patsubst src/%.c,build/obj/%.o,$(TEST_SRCS))
LIB_OBJS = $(filter-out $(TEST_OBJS),$(ALL_OBJS))

TEST_BINS = $(patsubst build/obj/%.o,build/test/%,$(TEST_OBJS))

ALL_DIRS = $(sort $(dir $(ALL_OBJS) $(TEST_BINS)))

RUN_TEST_BINS = $(addprefix run-,$(TEST_BINS))

.PHONY: all
all: $(ALL_OBJS)

include vendor/stb.mak

$(ALL_DIRS):
	mkdir -p $@

$(ALL_OBJS): build/obj/%.o: src/%.c | $(ALL_DIRS)
	$(CCACHE) $(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -c -o $@

$(TEST_BINS): build/test/%: build/obj/%.o $(LIB_OBJS) | $(ALL_DIRS)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: test
test: $(RUN_TEST_BINS)

$(RUN_TEST_BINS): run-%: %
	$<

.PHONY: clang-format
clang-format:
	clang-format --style=file -i $(ALL_SRCS)

.PHONY: clean
clean:
	rm -rf build
