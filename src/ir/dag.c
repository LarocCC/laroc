#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "ir/ctx.h"
#include "ir/func.h"
#include "ir/inst.h"
#include "ir/value.h"

static void buildDAGForFunc(IRCtx *ctx, IRFunc *func);
static void buildDAGForInst(IRCtx *ctx, IRInst *inst);
static Value *buildDAGForValue(IRCtx *ctx, Value *val);

static void removeTaggedInstFromBlock(IRCtx *ctx, IRBlock *blk);

void buildDAG(Module *mod) {
  IRCtx *ctx = newIRCtx(mod);
  ctx->funcVisitor = buildDAGForFunc;
  ctx->instVisitor = buildDAGForInst;
  ctx->valueVisitor = buildDAGForValue;
  visitIR(ctx);
  free(ctx);

  ctx = newIRCtx(mod);
  ctx->blockVisitor = removeTaggedInstFromBlock;
  visitIR(ctx);
  free(ctx);
}

static void buildDAGForFunc(IRCtx *ctx, IRFunc *func) {
  arrsetlen(func->instForValues, func->valueCount + 1);
  memset(func->instForValues, 0, sizeof(IRInst *) * (func->valueCount + 1));

  for (int i = 0; i < arrlen(func->allocas); i++) {
    IRInst *alloca = func->allocas[i];
    func->instForValues[alloca->dst->id] = alloca;
  }
}

static void buildDAGForInst(IRCtx *ctx, IRInst *inst) {
  assert(inst->kind != IR_ALLOCA && "Alloca in function body is not supported");
  switch (inst->kind) {
  case IR_LOAD:
  case IR_STORE:
  case IR_J:
  case IR_BR:
  case IR_RET:
    inst->isDAGRoot = true;
    break;

  default:
    inst->toBeRemoved = true;
  }

  if (inst->dst) {
    assert(inst->dst->kind == IR_VAL_VAR);
    ctx->func->instForValues[inst->dst->id] = inst;
  }
}

static Value *buildDAGForValue(IRCtx *ctx, Value *val) {
  if (val == ctx->inst->dst || val->kind != IR_VAL_VAR)
    return val;

  IRInst *valInst = ctx->func->instForValues[val->id];
  if (!valInst || valInst->isDAGRoot)
    return val;
  if (valInst->block != ctx->block) {
    valInst->toBeRemoved = false;
    return val;
  }
  return newValueDAGNode(valInst);
}

static void removeTaggedInstFromBlock(IRCtx *ctx, IRBlock *blk) {
  IRInst *inst = blk->instHead->next;
  while (inst != blk->instTail) {
    IRInst *nextInst = inst->next;
    if (inst->toBeRemoved) {
      irBlockRemoveInst(inst);
      inst->toBeRemoved = false;
    }
    inst = nextInst;
  }
}
