#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/block.h"
#include "riscv/func.h"
#include "riscv/inst.h"
#include "riscv/objfile.h"
#include "riscv/rvctx.h"

static void visitRVFunc(RVCtx *ctx, RVFunc *func);
static void visitRVBlock(RVCtx *ctx, RVBlock *block);
static void visitRVInst(RVCtx *ctx, RVInst *inst);

RVCtx *newRVCtx(ObjectFile *objFile) {
  static int maxVisitID = 0;

  RVCtx *ctx = calloc(1, sizeof(RVCtx));
  ctx->visitID = ++maxVisitID;
  ctx->objFile = objFile;
  return ctx;
}

void visitObjectFile(RVCtx *ctx) {
  for (int i = 0; i < arrlen(ctx->objFile->funcs); i++)
    visitRVFunc(ctx, ctx->objFile->funcs[i]);
}

static void visitRVFunc(RVCtx *ctx, RVFunc *func) {
  ctx->func = func;

  if (ctx->funcVisitor != NULL)
    ctx->funcVisitor(ctx, func);

  if (ctx->blockVisitor != NULL || ctx->instVisitor != NULL
      || ctx->operandVisitor != NULL)
    visitRVBlock(ctx, func->entry);

  ctx->func = NULL;
}

static void visitRVBlock(RVCtx *ctx, RVBlock *block) {
  if (block->lastVisitID == ctx->visitID)
    return;
  block->lastVisitID = ctx->visitID;
  ctx->block = block;

  if (ctx->blockVisitor != NULL)
    ctx->blockVisitor(ctx, block);

  if (ctx->instVisitor != NULL || ctx->operandVisitor != NULL) {
    for (RVInst *inst = block->instHead->next; inst != block->instTail;
         inst = inst->next) {
      visitRVInst(ctx, inst);
    }
  }

  ctx->block = NULL;

  for (int i = 0; i < arrlen(block->succs); i++)
    visitRVBlock(ctx, block->succs[i]);
}

static void visitRVInst(RVCtx *ctx, RVInst *inst) {
  ctx->inst = inst;

  if (ctx->instVisitor != NULL)
    ctx->instVisitor(ctx, inst);

  if (ctx->operandVisitor != NULL) {
    for (int i = 0; i < arrlen(inst->operands); i++) {
      Operand *retOp = ctx->operandVisitor(ctx, inst->operands[i]);
      if (retOp != inst->operands[i]) {
        inst->operands[i] = retOp;
      }
    }
  }

  ctx->inst = NULL;
}
