#ifndef LAROC_IRGEN_IRGEN_H
#define LAROC_IRGEN_IRGEN_H

#include <stdbool.h>

#include "typedef.h"

struct IRGenCtx {
  Declaration *cFunc;
  SymTable *symtab;

  IRFunc *irFunc;
  IRBlock *block;

  bool unreachable;
};

Module *genIR(TranslationUnit *unit);

#endif
