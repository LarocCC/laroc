TOKEN_OUTPUTS = $(wildcard test/lex/*.tokens.txt)
CHECK_TOKEN_INPUTS = $(addprefix check-,$(TOKEN_OUTPUTS))

test-lex: $(CHECK_TOKEN_INPUTS)
$(CHECK_TOKEN_INPUTS): check-%.tokens.txt: %.c | $(LAORC_BINS)
	./build/bin/laroc -print-tokens $< > $(patsubst test/%.c,build/test/%.tokens.txt,$<)
	diff -u $(patsubst test/%.c,test/%.tokens.txt,$<) $(patsubst test/%.c,build/test/%.tokens.txt,$<)
