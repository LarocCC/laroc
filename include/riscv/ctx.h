#ifndef LAROC_RISCV_RVCTX_H
#define LAROC_RISCV_RVCTX_H

#include <stdbool.h>

#include "typedef.h"
#include "util/visitord.h"

typedef void RVFuncVisitor(RVCtx *ctx, RVFunc *func);
typedef bool RVBlockVisitor(RVCtx *ctx, RVBlock *block);
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
  RVBlockVisitor *blockVisitorAfter;
  RVFuncVisitor *funcVisitorAfter;

  VisitOrder blockVisitOrder;
  VisitOrder instVisitOrder;

  void *data;
};

RVCtx *newRVCtx(ObjectFile *objFile);

void visitObjectFile(RVCtx *ctx);

#endif
