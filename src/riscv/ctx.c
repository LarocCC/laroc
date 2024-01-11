#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/block.h"
#include "riscv/ctx.h"
#include "riscv/func.h"
#include "riscv/inst.h"
#include "riscv/objfile.h"
#include "util/visitord.h"

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

  if (ctx->funcVisitor)
    ctx->funcVisitor(ctx, func);

  if (ctx->blockVisitor || ctx->instVisitor || ctx->operandVisitor) {
    switch (ctx->blockVisitOrder) {
    case VISIT_ORD_DEFAULT:
    case VISIT_ORD_DEFAULT_UNTIL_UNCHANGED:
      visitRVBlock(ctx, func->entry);
      break;

    case VISIT_ORD_REVERSE:
    case VISIT_ORD_REVERSE_UNTIL_UNCHANGED:
      for (int i = 0; i < arrlen(func->exits); i++)
        visitRVBlock(ctx, func->exits[i]);
      break;

    default:
      assert(false);
    }
  }

  if (ctx->funcVisitorAfter)
    ctx->funcVisitorAfter(ctx, func);

  ctx->func = NULL;
}

static void visitRVBlock(RVCtx *ctx, RVBlock *block) {
  switch (ctx->blockVisitOrder) {
  case VISIT_ORD_DEFAULT:
  case VISIT_ORD_REVERSE:
    if (block->lastVisitID == ctx->visitID)
      return;
  default:
    break;
  }

  block->lastVisitID = ctx->visitID;
  ctx->block = block;

  if (ctx->blockVisitor)
    ctx->blockVisitor(ctx, block);

  if (ctx->instVisitor || ctx->operandVisitor) {
    switch (ctx->instVisitOrder) {
    case VISIT_ORD_DEFAULT:
      for (RVInst *inst = block->instHead->next; inst != block->instTail;
           inst = inst->next) {
        visitRVInst(ctx, inst);
      }
      break;

    case VISIT_ORD_REVERSE:
      for (RVInst *inst = block->instTail->prev; inst != block->instHead;
           inst = inst->prev) {
        visitRVInst(ctx, inst);
      }
      break;

    default:
      assert(false);
    }
  }

  bool changed = false;
  if (ctx->blockVisitorAfter)
    changed = ctx->blockVisitorAfter(ctx, block);

  ctx->block = NULL;

  switch (ctx->blockVisitOrder) {
  case VISIT_ORD_DEFAULT_UNTIL_UNCHANGED:
    if (!changed)
      return;
    // fallthrough
  case VISIT_ORD_DEFAULT:
    for (int i = 0; i < arrlen(block->succs); i++)
      visitRVBlock(ctx, block->succs[i]);
    break;

  case VISIT_ORD_REVERSE_UNTIL_UNCHANGED:
    if (!changed)
      return;
  case VISIT_ORD_REVERSE:
    for (int i = 0; i < arrlen(block->preds); i++)
      visitRVBlock(ctx, block->preds[i]);
    break;

  default:
    assert(false);
  }
}

static void visitRVInst(RVCtx *ctx, RVInst *inst) {
  ctx->inst = inst;

  if (ctx->instVisitor)
    ctx->instVisitor(ctx, inst);

  if (ctx->operandVisitor) {
    for (int i = 0; i < arrlen(inst->operands); i++) {
      Operand *retOp = ctx->operandVisitor(ctx, inst->operands[i]);
      if (retOp != inst->operands[i]) {
        inst->operands[i] = retOp;
      }
    }
  }

  ctx->inst = NULL;
}
