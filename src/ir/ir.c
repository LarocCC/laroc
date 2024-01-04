#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/inst.h"
#include "ir/ir.h"
#include "ir/module.h"

static void visitIRFunc(IRCtx *ctx, IRFunc *func);
static void visitIRBlock(IRCtx *ctx, IRBlock *block);
static void visitIRInst(IRCtx *ctx, IRInst *inst);

IRCtx *newIRCtx(Module *mod) {
  static int maxVisitID = 0;

  IRCtx *ctx = calloc(1, sizeof(IRCtx));
  ctx->visitID = ++maxVisitID;
  ctx->mod = mod;

  return ctx;
}

void visitIR(IRCtx *ctx) {
  for (int i = 0; i < arrlen(ctx->mod->funcs); i++)
    visitIRFunc(ctx, ctx->mod->funcs[i]);
}

static void visitIRFunc(IRCtx *ctx, IRFunc *func) {
  ctx->func = func;

  if (ctx->funcVisitor != NULL)
    ctx->funcVisitor(ctx, func);

  if (ctx->blockVisitor != NULL || ctx->instVisitor != NULL
      || ctx->valueVisitor != NULL)
    visitIRBlock(ctx, func->entry);

  ctx->func = NULL;
}

static void visitIRBlock(IRCtx *ctx, IRBlock *block) {
  if (block->lastVisitID == ctx->visitID)
    return;
  block->lastVisitID = ctx->visitID;
  ctx->block = block;

  if (ctx->blockVisitor != NULL)
    ctx->blockVisitor(ctx, block);

  if (ctx->instVisitor != NULL || ctx->valueVisitor != NULL) {
    for (IRInst *inst = block->instHead->next; inst != block->instTail;
         inst = inst->next) {
      visitIRInst(ctx, inst);
    }
  }

  ctx->block = NULL;

  for (int i = 0; i < arrlen(block->succs); i++)
    visitIRBlock(ctx, block->succs[i]);
}

static void visitIRInst(IRCtx *ctx, IRInst *inst) {
  ctx->inst = inst;

  if (ctx->instVisitor != NULL)
    ctx->instVisitor(ctx, inst);

  if (ctx->valueVisitor != NULL) {
    if (inst->dst != NULL) {
      Value *retVal = ctx->valueVisitor(ctx, inst->dst);
      if (retVal != inst->dst)
        inst->dst = retVal;
    }

    for (int i = 0; i < arrlen(inst->srcs); i++) {
      Value *retVal = ctx->valueVisitor(ctx, inst->srcs[i]);
      if (retVal != inst->srcs[i]) {
        inst->srcs[i] = retVal;
      }
    }
  }

  ctx->inst = NULL;
}
