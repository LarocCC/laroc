#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"

static void buildDAGForFunc(IRCtx *ctx, IRFunc *func);
static void buildDAGForBlock(IRCtx *ctx, IRBlock *blk);

void buildDAG(Module *mod) {
  IRCtx ctx;
  memset(&ctx, 0, sizeof(IRCtx));

  for (int i = 0; i < arrlen(mod->funcs); i++)
    buildDAGForFunc(&ctx, mod->funcs[i]);
}

static void buildDAGForFunc(IRCtx *ctx, IRFunc *func) {
  ctx->func = func;

  arrsetlen(func->instForValues, func->valueCount + 1);
  memset(func->instForValues, 0, sizeof(IRInst *) * (func->valueCount + 1));

  for (int i = 0; i < arrlen(func->allocas); i++) {
    IRInst *alloca = func->allocas[i];
    func->instForValues[alloca->dst->id] = alloca;
  }

  buildDAGForBlock(ctx, func->entry);

  ctx->func = NULL;
}

static void buildDAGForBlock(IRCtx *ctx, IRBlock *blk) {
  IRInst *inst = blk->instHead->next;

  while (inst != blk->instTail) {
    assert(inst->kind != IR_ALLOCA);

    if (inst->kind != IR_LOAD && inst->dst != NULL) {
      assert(inst->dst->kind == IR_VAL_VAR);
      ctx->func->instForValues[inst->dst->id] = inst;
    }

    for (int i = 0; i < arrlen(inst->srcs); i++) {
      Value *src = inst->srcs[i];
      if (src->kind != IR_VAL_VAR)
        continue;
      IRInst *srcInst = ctx->func->instForValues[src->id];
      if (srcInst != NULL && srcInst->block == blk) {
        inst->srcs[i] = newValueDAGNode(srcInst);
        inst->srcs[i]->id = srcInst->dst->id;
      }
    }

    IRInst *nextInst = inst->next;
    switch (inst->kind) {
    case IR_LOAD:
    case IR_STORE:
    case IR_J:
    case IR_RET:
      inst = inst->next;
      break;

    default:
      irBlockRemoveInst(inst);
      inst->block = blk;
    }
    inst = nextInst;
  }
}
