#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"

static void buildDAGForBlock(IRCtx *ctx, Block *blk);

void buildDAG(Module *mod) {
  IRCtx ctx;
  memset(&ctx, 0, sizeof(IRCtx));

  for (int i = 0; i < arrlen(mod->funcs); i++) {
    Func *func = ctx.func = mod->funcs[i];

    arrsetlen(func->instForValues, func->valueCount + 1);
    memset(func->instForValues, 0, sizeof(IRInst *) * (func->valueCount + 1));

    buildDAGForBlock(&ctx, func->entry);
  }
}

static void buildDAGForBlock(IRCtx *ctx, Block *blk) {
  for (int i = 0; i < arrlen(blk->insts); i++) {
    IRInst *inst = blk->insts[i];

    switch (inst->kind) {
    case IR_ALLOCA:
      assert(false);

    case IR_LOAD:
    case IR_STORE:
    case IR_RET:
      inst->isDAGRoot = true;
      break;

    default:
      break;
    }

    if (inst->dst != NULL) {
      assert(inst->dst->kind == IR_VAL_VAR);
      ctx->func->instForValues[inst->dst->id] = inst;
    }
  }
}
