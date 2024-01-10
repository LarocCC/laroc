#ifndef LAROC_IR_FUNC_H
#define LAROC_IR_FUNC_H

#include "typedef.h"

/// An IR function.
struct IRFunc {
  const char *name;
  IRType *ret;
  Value **args;

  /// Static on-stack allocations. This is a list of alloca instrunctions.
  IRInst **allocas;

  int blockCount;
  IRBlock **blocks;
  IRBlock *entry;
  IRBlock **exits;

  int valueCount;
  IRInst **instForValues;
};

IRFunc *newIRFunc(const char *name);

void printIRFunc(IRFunc *func);

#endif
