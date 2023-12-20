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

int main(int argc, char *argv[]) {
  CLIOpt *opt = parseArgs(argc, argv);

  if (arrlen(opt->inputs) != 1) {
    printf("Usage: %s input.c\n", argv[0]);
    return 1;
  }

  int fd = open(opt->inputs[0], O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  int len = lseek(fd, 0, SEEK_END);
  if (len == -1) {
    perror("lseek");
    return 1;
  }
  if (len == 0) {
    return 0;
  }

  const char *source = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
  if (source == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  Token *tokens = lex(source, len);

  TranslationUnit *unit = parseTranslationUnit(tokens);

  Module *mod = genIR(unit);
  printModule(mod);

  return 0;
}
