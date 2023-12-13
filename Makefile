CCACHE = ccache
CC = clang

CFLAGS = -std=c99 -Wall -Wextra -g

INCLUDE_DIRS = -Isrc -Ivendor/stb

ALL_SRCS = $(wildcard src/*/*.c)
TEST_SRCS = $(wildcard src/*/*.test.c)

ALL_DEP_FILES = $(patsubst src/%.c,build/dep/%.d,$(ALL_SRCS))

ALL_OBJS = $(patsubst src/%.c,build/obj/%.o,$(ALL_SRCS))
TEST_OBJS = $(patsubst src/%.c,build/obj/%.o,$(TEST_SRCS))
LIB_OBJS = $(filter-out $(TEST_OBJS),$(ALL_OBJS))

TEST_BINS = $(patsubst build/obj/%.o,build/test/%,$(TEST_OBJS))

ALL_DIRS = $(sort $(dir $(ALL_DEP_FILES) $(ALL_OBJS) $(TEST_BINS)))

RUN_TEST_BINS = $(addprefix run-,$(TEST_BINS))

.PHONY: all
all: $(ALL_OBJS)

include vendor/stb.mak
-include $(ALL_DEP_FILES)

$(ALL_DIRS):
	mkdir -p $@

.PHONY: dep
dep: $(ALL_DEP_FILES)
$(ALL_DEP_FILES): build/dep/%.d: src/%.c | $(ALL_DIRS)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -MM -MT $(patsubst build/dep/%.d,build/obj/%.o,$@) -o $@

$(ALL_OBJS): | $(ALL_DIRS)
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

.PHONY: clean-dep
clean-dep:
	rm -rf build/dep

.PHONY: clean
clean: clean-dep
	rm -rf build
