#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "lex/lex.h"
#include "parse/parse.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s input.c\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];
  int fd = open(filename, O_RDONLY);
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
