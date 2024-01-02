#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"

static void buildDAGForBlock(IRCtx *ctx, IRBlock *blk);

void buildDAG(Module *mod) {
  IRCtx ctx;
  memset(&ctx, 0, sizeof(IRCtx));

  for (int i = 0; i < arrlen(mod->funcs); i++) {
    IRFunc *func = ctx.func = mod->funcs[i];

    arrsetlen(func->instForValues, func->valueCount + 1);
    memset(func->instForValues, 0, sizeof(IRInst *) * (func->valueCount + 1));

    for (int i = 0; i < arrlen(func->allocas); i++) {
      IRInst *alloca = func->allocas[i];
      func->instForValues[alloca->dst->id] = alloca;
    }

    buildDAGForBlock(&ctx, func->entry);
  }
}

static void buildDAGForBlock(IRCtx *ctx, IRBlock *blk) {
  IRInst *inst = blk->instHead->next;
  while (inst != blk->instTail) {
    assert(inst->kind != IR_ALLOCA);

    if (inst->kind != IR_LOAD && inst->dst != NULL) {
      assert(inst->dst->kind == IR_VAL_VAR);
      ctx->func->instForValues[inst->dst->id] = inst;
    }

    if (inst->src1 != NULL && inst->src1->kind == IR_VAL_VAR) {
      IRInst *src1Inst = ctx->func->instForValues[inst->src1->id];
      if (src1Inst != NULL && src1Inst->block == blk)
        inst->src1 = newValueDAGNode(src1Inst);
    }
    if (inst->src2 != NULL && inst->src2->kind == IR_VAL_VAR) {
      IRInst *src2Inst = ctx->func->instForValues[inst->src2->id];
      if (src2Inst != NULL && src2Inst->block == blk)
        inst->src2 = newValueDAGNode(src2Inst);
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
