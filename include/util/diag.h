#ifndef LAROC_UTIL_DIAG_H
#define LAROC_UTIL_DIAG_H

#include "typedef.h"

struct SourceLoc {
  int lineno, col;
};

SourceLoc *newSourceLoc(int lineno, int col);

void emitDiagnostic(SourceLoc *loc, const char *fmt, ...);

#endif
