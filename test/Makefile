TEST_INPUTS = $(wildcard test/*/*.c test/*/*/*.c)
TEST_XFAIL_INPUTS = $(wildcard test/*/*.xfail.c test/*/*/*.xfail.c)
TEST_XPASS_INPUTS = $(filter-out $(TEST_XFAIL_INPUTS),$(TEST_INPUTS))

TEST_OUTPUTS = $(patsubst test/%.c,build/test/%.out.txt,$(TEST_INPUTS))
TEST_EXPECT_OUTPUTS = $(wildcard test/*/*.out.txt test/*/*/*.out.txt)

RUN_WITH_TEST_XPASS_INPUTS = $(addprefix run-with-,$(TEST_XPASS_INPUTS))
$(RUN_WITH_TEST_XPASS_INPUTS): run-with-%: % | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-after=$(wordlist 2,2,$(subst /, ,$<)) $< > $(patsubst test/%.c,build/test/%.out.txt,$<)

CHECK_TEST_EXPECT_OUTPUTS = $(addprefix check-,$(TEST_EXPECT_OUTPUTS))
$(CHECK_TEST_EXPECT_OUTPUTS): check-%: % build/% | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	diff -u $< $(patsubst test/%.out.txt,build/test/%.out.txt,$<)

CHECK_TEST_XFAIL_INPUTS = $(addprefix check-,$(TEST_XFAIL_INPUTS))
$(CHECK_TEST_XFAIL_INPUTS): check-%: % | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	! ./build/bin/laroc -print-after=$(wordlist 2,2,$(subst /, ,$<)) $<

.PHONY: test-inputs
test-inputs: $(CHECK_TEST_XFAIL_INPUTS) $(RUN_WITH_TEST_XPASS_INPUTS) $(CHECK_TEST_EXPECT_OUTPUTS)

ALL_DIRS += $(dir $(TEST_OUTPUTS))
