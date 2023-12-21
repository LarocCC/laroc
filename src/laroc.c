#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "lex/lex.h"
#include "lex/token.h"
#include "parse/parse.h"
#include "util/argparse.h"
#include "util/file.h"

int main(int argc, char *argv[]) {
  CLIOpt *opt = parseArgs(argc, argv);

  if (arrlen(opt->inputs) != 1) {
    printf("Usage: %s input.c\n", argv[0]);
    return 1;
  }

  int len;
  const char *source = readFile(opt->inputs[0], &len);

  Token *tokens = lex(source, len);
  if (opt->printToken) {
    for (int i = 0; i < arrlen(tokens); i++)
      printToken(&tokens[i]);
    return 0;
  }

  TranslationUnit *unit = parseTranslationUnit(tokens);
  if (opt->printAST) {
    printTranslationUnit(unit);
    return 0;
  }

  Module *mod = genIR(unit);
  if (opt->printIR) {
    printModule(mod);
    return 0;
  }

  return 0;
}