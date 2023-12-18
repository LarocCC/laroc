#ifndef LAROC_IRGEN_IRGEN_H
#define LAROC_IRGEN_IRGEN_H

#include "typedef.h"

struct IRGenCtx {
  SymTable *symtab;

  Func *func;
  Block *block;
};

Module *generateIR(TranslationUnit *unit);

IRType *newIRTypeFromCType(CType *cty);

#endif
