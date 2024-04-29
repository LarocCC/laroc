#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "util/diag.h"

SourceLoc *newSourceLoc(const char *filename, int lineno, int col) {
  SourceLoc *loc = calloc(1, sizeof(SourceLoc));
  loc->filename = filename;
  loc->lineno = lineno;
  loc->col = col;
  return loc;
}

SourceLoc *copySourceLoc(SourceLoc *loc) {
  SourceLoc *ret = calloc(1, sizeof(SourceLoc));
  memcpy(ret, loc, sizeof(SourceLoc));
  return ret;
}

void emitDiagnostic(SourceLoc *loc, const char *fmt, ...) {
  printf("%s:%d:%d: ", loc->filename, loc->lineno, loc->col);

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");

  exit(1);
}
