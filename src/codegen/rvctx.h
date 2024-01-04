#ifndef LAROC_CODEGEN_RVCTX_H
#define LAROC_CODEGEN_RVCTX_H

#include "typedef.h"

typedef void RVFuncVisitor(RVCtx *ctx, RVFunc *func);
typedef void RVBlockVisitor(RVCtx *ctx, RVBlock *block);
typedef void RVInstVisitor(RVCtx *ctx, RVInst *inst);
typedef Operand *OperandVisitor(RVCtx *ctx, Operand *val);

struct RVCtx {
  int visitID;

  ObjectFile *objFile;
  RVFunc *func;
  RVBlock *block;
  RVInst *inst;

  RVFuncVisitor *funcVisitor;
  RVBlockVisitor *blockVisitor;
  RVInstVisitor *instVisitor;
  OperandVisitor *operandVisitor;
};

RVCtx *newRVCtx(ObjectFile *objFile);

void visitObjectFile(RVCtx *ctx);

#endif
