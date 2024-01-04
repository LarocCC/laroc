#ifndef LAROC_IR_MODULE_H
#define LAROC_IR_MODULE_H

#include "typedef.h"

struct Module {
  IRFunc **funcs;
};

void printModule(Module *mod);

struct IRFunc {
  const char *name;
  IRType *ret;
  Value **args;

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

struct IRBlock {
  int id;
  IRBlock **preds, **succs;

  IRInst *instHead, *instTail;

  int lastVisitID;
};

IRBlock *newIRBlock(IRFunc *func);

void printIRBlock(IRBlock *blk);


#endif
