TOKEN_OUTPUTS = $(wildcard test/lex/*.tokens.txt)
CHECK_TOKEN_OUTPUTS = $(addprefix check-,$(TOKEN_OUTPUTS))

.PHONY: test-lex
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

.PHONY: test-parse
test-parse: $(CHECK_AST_OUTPUTS) $(CHECK_PARSE_XFAIL_SRCS)

IR_OUTPUTS = $(wildcard test/irgen/*.ir.txt)
CHECK_IR_OUTPUTS = $(addprefix check-,$(IR_OUTPUTS))

.PHONY: test-irgen
test-irgen: $(CHECK_IR_OUTPUTS)
$(CHECK_IR_OUTPUTS): check-%.ir.txt: %.c | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-ir $< > $(patsubst test/%.c,build/test/%.ir.txt,$<)
	diff -u $(patsubst test/%.c,test/%.ir.txt,$<) $(patsubst test/%.c,build/test/%.ir.txt,$<)

DAG_OUTPUTS = $(wildcard test/dag/*.ir.txt)
CHECK_DAG_OUTPUTS = $(addprefix check-,$(DAG_OUTPUTS))

.PHONY: test-dag
test-dag: $(CHECK_DAG_OUTPUTS)
$(CHECK_DAG_OUTPUTS): check-%.ir.txt: %.c | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-dag $< > $(patsubst test/%.c,build/test/%.ir.txt,$<)
	diff -u $(patsubst test/%.c,test/%.ir.txt,$<) $(patsubst test/%.c,build/test/%.ir.txt,$<)

ISEL_OUTPUTS = $(wildcard test/isel/*.s)
CHECK_ISEL_OUTPUTS = $(addprefix check-,$(ISEL_OUTPUTS))

.PHONY: test-isel
test-isel: $(CHECK_ISEL_OUTPUTS)
$(CHECK_ISEL_OUTPUTS): check-%.s: %.c | $(LAORC_BINS) $(SORTED_ALL_DIRS)
	./build/bin/laroc -print-after-isel $< > $(patsubst test/%.c,build/test/%.s,$<)
	diff -u $(patsubst test/%.c,test/%.s,$<) $(patsubst test/%.c,build/test/%.s,$<)

.PHONY: test-inputs
test-inputs: test-lex test-parse test-irgen test-dag test-isel

ALL_DIRS += $(dir $(addprefix build/,$(TOKEN_OUTPUTS) $(AST_OUTPUTS) $(IR_OUTPUTS) $(DAG_OUTPUTS) $(ISEL_OUTPUTS)))
