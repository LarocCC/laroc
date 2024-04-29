#ifndef LAROC_UTIL_DIAG_H
#define LAROC_UTIL_DIAG_H

#include "typedef.h"

struct SourceLoc {
  const char *filename;
  int lineno, col;
};

SourceLoc *newSourceLoc(const char *filename, int lineno, int col);

SourceLoc *copySourceLoc(SourceLoc *loc);

void emitDiagnostic(SourceLoc *loc, const char *fmt, ...);

#endif
