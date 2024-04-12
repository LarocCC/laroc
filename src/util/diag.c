#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "util/diag.h"

SourceLoc *newSourceLoc(int lineno, int col) {
  SourceLoc *loc = calloc(1, sizeof(SourceLoc));
  loc->lineno = lineno;
  loc->col = col;
  return loc;
}

void emitDiagnostic(SourceLoc *loc, const char *fmt, ...) {
  printf("Error at line %d column %d: ", loc->lineno, loc->col);

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");

  exit(1);
}
