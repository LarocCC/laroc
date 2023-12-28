CCACHE = ccache
CC = clang

CFLAGS = -std=c99 -Wall -Wextra -g

INCLUDE_DIRS = -Isrc -Ivendor

ALL_C_SRCS = $(wildcard src/*.c src/*/*.c)
ALL_H_SRCS = $(wildcard src/*.h src/*/*.h)
ALL_SRCS = $(ALL_C_SRCS) $(ALL_H_SRCS)
BIN_SRCS = src/laroc.c
TEST_SRCS = $(wildcard src/*/*.test.c)

ALL_DEP_FILES = $(patsubst src/%.c,build/dep/%.d,$(ALL_C_SRCS))

ALL_OBJS = $(patsubst src/%.c,build/obj/%.o,$(ALL_C_SRCS))
BIN_OBJS = $(patsubst src/%.c,build/obj/%.o,$(BIN_SRCS))
TEST_OBJS = $(patsubst src/%.c,build/obj/%.o,$(TEST_SRCS))
LIB_OBJS = $(filter-out $(BIN_OBJS) $(TEST_OBJS),$(ALL_OBJS))

LAORC_BINS = $(patsubst src/%.c,build/bin/%,$(BIN_SRCS))
TEST_BINS = $(patsubst build/obj/%.o,build/test/%,$(TEST_OBJS))

ALL_DIRS = $(dir $(ALL_DEP_FILES) $(ALL_OBJS) $(LAORC_BINS) $(TEST_BINS))

RUN_TEST_BINS = $(addprefix run-,$(TEST_BINS))

.PHONY: all
all: $(LAORC_BINS) $(ALL_OBJS)

include vendor/stb.mak
include test/Makefile
-include $(ALL_DEP_FILES)

SORTED_ALL_DIRS = $(sort $(ALL_DIRS))
$(SORTED_ALL_DIRS):
	mkdir -p $@

.PHONY: dep
dep: $(ALL_DEP_FILES)
$(ALL_DEP_FILES): build/dep/%.d: src/%.c | $(SORTED_ALL_DIRS)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -MM -MT $(patsubst build/dep/%.d,build/obj/%.o,$@) -o $@

$(ALL_OBJS): | $(SORTED_ALL_DIRS)
	$(CCACHE) $(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -c -o $@

$(TEST_BINS): build/test/%: build/obj/%.o $(LIB_OBJS) | $(SORTED_ALL_DIRS)
	$(CC) $(CFLAGS) $^ -o $@

$(LAORC_BINS): build/bin/%: build/obj/%.o $(LIB_OBJS) | $(SORTED_ALL_DIRS)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: test
test: $(RUN_TEST_BINS) test-lex test-parse test-irgen

.PHONY: $(RUN_TEST_BINS)
$(RUN_TEST_BINS): run-%: %
	$<

.PHONY: clang-format
clang-format:
	clang-format --style=file -i $(ALL_SRCS)

.PHONY: clean-dep
clean-dep:
	rm -rf build/dep

.PHONY: clean
clean: clean-dep
	rm -rf build
