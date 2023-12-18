#ifndef LAROC_IRGEN_FUNC_H
#define LAROC_IRGEN_FUNC_H

#include "typedef.h"

struct Func {
  const char *name;
  IRType *ret;
  Val **args;

  IRInst **allocas;

  Block *entry;
  Block **exits;
};

Func *newFunc(const char *name);

Func *generateFunc(IRGenCtx *ctx, Declaration *decl);

#endif
