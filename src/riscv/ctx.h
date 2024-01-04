#ifndef LAROC_RISCV_RVCTX_H
#define LAROC_RISCV_RVCTX_H

#include "typedef.h"
#include "util/visitord.h"

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

  VisitOrder blockVisitOrder;
  VisitOrder instVisitOrder;
};

RVCtx *newRVCtx(ObjectFile *objFile);

void visitObjectFile(RVCtx *ctx);

#endif
