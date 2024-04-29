TEST_INPUTS = $(wildcard test/*/*.c test/*/*/*.c)
TEST_XFAIL_INPUTS = $(wildcard test/*/*.xfail.c test/*/*/*.xfail.c)
TEST_XPASS_INPUTS = $(filter-out $(TEST_XFAIL_INPUTS),$(TEST_INPUTS))

TEST_OUTPUTS = $(patsubst test/%.c,build/test/%.out.txt,$(TEST_INPUTS))
TEST_EXPECT_OUTPUTS = $(wildcard test/*/*.out.txt test/*/*/*.out.txt)

TEST_XPASS_OUTPUTS = $(patsubst test/%.c,build/test/%.out.txt,$(TEST_XPASS_INPUTS))
$(TEST_XPASS_OUTPUTS): build/test/%.out.txt: test/%.c $(LAORC_BINS) | $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-after=$(wordlist 2,2,$(subst /, ,$<)) $< > $@

CHECK_TEST_EXPECT_OUTPUTS = $(addprefix check-,$(TEST_EXPECT_OUTPUTS))
$(CHECK_TEST_EXPECT_OUTPUTS): check-%: % build/% | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	diff -u $< $(patsubst test/%.out.txt,build/test/%.out.txt,$<)

CHECK_TEST_XFAIL_INPUTS = $(addprefix check-,$(TEST_XFAIL_INPUTS))
$(CHECK_TEST_XFAIL_INPUTS): check-%: % | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	! ./build/bin/laroc -print-after=$(wordlist 2,2,$(subst /, ,$<)) $< > /dev/null

.PHONY: test-inputs
test-inputs: $(CHECK_TEST_XFAIL_INPUTS) $(TEST_XPASS_OUTPUTS) $(CHECK_TEST_EXPECT_OUTPUTS)

ALL_DIRS += $(dir $(TEST_OUTPUTS))

UPDATE_TEST_OUTPUTS = $(patsubst test/%.c,update-test/%.out.txt,$(TEST_XPASS_INPUTS))

.PHONY: update-test-outputs
update-test-outputs: $(UPDATE_TEST_OUTPUTS)
$(UPDATE_TEST_OUTPUTS): update-%: build/%
	cp $< $(patsubst build/%,%,$<)
