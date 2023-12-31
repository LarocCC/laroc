TOKEN_OUTPUTS = $(wildcard test/lex/*.tokens.txt)
CHECK_TOKEN_OUTPUTS = $(addprefix check-,$(TOKEN_OUTPUTS))

test-lex: $(CHECK_TOKEN_OUTPUTS)
$(CHECK_TOKEN_OUTPUTS): check-%.tokens.txt: %.c | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-tokens $< > $(patsubst test/%.c,build/test/%.tokens.txt,$<)
	diff -u $(patsubst test/%.c,test/%.tokens.txt,$<) $(patsubst test/%.c,build/test/%.tokens.txt,$<)

AST_OUTPUTS = $(wildcard test/parse/*.ast.txt test/parse/*/*.ast.txt)
CHECK_AST_OUTPUTS = $(addprefix check-,$(AST_OUTPUTS))

$(CHECK_AST_OUTPUTS): check-%.ast.txt: %.c | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-ast $< > $(patsubst test/%.c,build/test/%.ast.txt,$<)
	diff -u $(patsubst test/%.c,test/%.ast.txt,$<) $(patsubst test/%.c,build/test/%.ast.txt,$<)

PARSE_XFAIL_SRCS = $(wildcard test/parse/*.xfail.c test/parse/*/*.xfail.c)
CHECK_PARSE_XFAIL_SRCS = $(addprefix check-,$(PARSE_XFAIL_SRCS))

$(CHECK_PARSE_XFAIL_SRCS): check-%: % | $(LAORC_BINS)
	! ./build/bin/laroc -print-ast $<

test-parse: $(CHECK_AST_OUTPUTS) $(CHECK_PARSE_XFAIL_SRCS)

IR_OUTPUTS = $(wildcard test/irgen/*.ir.txt)
CHECK_IR_OUTPUTS = $(addprefix check-,$(IR_OUTPUTS))

test-irgen: $(CHECK_IR_OUTPUTS)
$(CHECK_IR_OUTPUTS): check-%.ir.txt: %.c | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-ir $< > $(patsubst test/%.c,build/test/%.ir.txt,$<)
	diff -u $(patsubst test/%.c,test/%.ir.txt,$<) $(patsubst test/%.c,build/test/%.ir.txt,$<)

ALL_DIRS += $(dir $(addprefix build/,$(TOKEN_OUTPUTS) $(AST_OUTPUTS) $(IR_OUTPUTS)))
