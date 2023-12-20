#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "lex/lex.h"
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

  TranslationUnit *unit = parseTranslationUnit(tokens);

  Module *mod = genIR(unit);
  printModule(mod);

  return 0;
}
