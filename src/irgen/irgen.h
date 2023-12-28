#ifndef LAROC_IRGEN_IRGEN_H
#define LAROC_IRGEN_IRGEN_H

#include "typedef.h"

struct IRGenCtx {
  SymTable *symtab;

  IRFunc *func;
  IRBlock *block;
};

Module *genIR(TranslationUnit *unit);

IRType *newIRTypeFromCType(CType *cty);

#endif
