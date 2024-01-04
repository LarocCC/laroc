#ifndef LAROC_IR_IR_H
#define LAROC_IR_IR_H

#include <stdbool.h>

#include "typedef.h"

typedef void IRFuncVisitor(IRCtx *ctx, IRFunc *func);
typedef void IRBlockVisitor(IRCtx *ctx, IRBlock *block);
typedef void IRInstVisitor(IRCtx *ctx, IRInst *inst);
typedef Value *ValueVisitor(IRCtx *ctx, Value *val);

struct IRCtx {
  int visitID;

  Module *mod;
  IRFunc *func;
  IRBlock *block;
  IRInst *inst;

  IRFuncVisitor *funcVisitor;
  IRBlockVisitor *blockVisitor;
  IRInstVisitor *instVisitor;
  ValueVisitor *valueVisitor;
};

IRCtx *newIRCtx(Module *mod);

void visitIR(IRCtx *ctx);

#endif
