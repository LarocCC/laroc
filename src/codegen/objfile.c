#include <stdlib.h>

#include "typedef.h"
#include "codegen/objfile.h"
#include "ir/ir.h"

RVFunc *newRVFunc(IRFunc *irFunc) {
  RVFunc *rvFunc = calloc(1, sizeof(RVFunc));
  rvFunc->name = irFunc->name;
  rvFunc->frameObjs = irFunc->allocas;
  rvFunc->blockCount = irFunc->blockCount;
  return rvFunc;
}

RVBlock *newRVBlock(IRBlock *irBlock) {
  RVBlock *rvBlock = calloc(1, sizeof(RVBlock));
  rvBlock->id = irBlock->id;
  return rvBlock;
}
