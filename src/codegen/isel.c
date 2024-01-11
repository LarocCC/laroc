#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "ir/func.h"
#include "ir/inst.h"
#include "ir/module.h"
#include "ir/type.h"
#include "ir/value.h"
#include "riscv/block.h"
#include "riscv/func.h"
#include "riscv/inst.h"
#include "riscv/objfile.h"
#include "riscv/operand.h"
#include "riscv/reg.h"

typedef struct ISelCtx {
  Module *mod;
  ObjectFile *objFile;

  IRFunc *irFunc;
  RVFunc *func;

  IRBlock *irBlock;
  RVBlock *block;
} ISelCtx;

static RVFunc *iselFunc(ISelCtx *ctx, IRFunc *irFunc);
static void iselArgs(ISelCtx *ctx);
static void iselBlock(ISelCtx *ctx, RVBlock *block, IRBlock *irBlock);
static void iselInst(ISelCtx *ctx, IRInst *irInst);

static void iselLoad(ISelCtx *ctx, IRInst *irInst);
static void iselStore(ISelCtx *ctx, IRInst *irInst);
static void iselAdd(ISelCtx *ctx, IRInst *irInst);
static void iselSub(ISelCtx *ctx, IRInst *irInst);
static void iselLi(ISelCtx *ctx, IRInst *irInst);
static void iselBr(ISelCtx *ctx, IRInst *irInst);
static void iselJ(ISelCtx *ctx, IRInst *irInst);
static void iselRet(ISelCtx *ctx, IRInst *irInst);

ObjectFile *selectInstruction(Module *mod) {
  ISelCtx ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.mod = mod;
  ctx.objFile = calloc(1, sizeof(ObjectFile));

  for (int i = 0; i < arrlen(mod->funcs); i++) {
    RVFunc *func = iselFunc(&ctx, mod->funcs[i]);
    arrput(ctx.objFile->funcs, func);
  }

  return ctx.objFile;
}

static RVFunc *iselFunc(ISelCtx *ctx, IRFunc *irFunc) {
  RVFunc *func = newRVFunc(irFunc);
  ctx->irFunc = irFunc;
  ctx->func = func;

  ctx->block = func->entry;
  iselArgs(ctx);
  ctx->block = NULL;

  for (int i = 1; i <= func->blockCount; i++)
    iselBlock(ctx, func->blocks[i], irFunc->blocks[i]);

  ctx->irFunc = NULL;
  ctx->func = NULL;
  return func;
}

static void iselArgs(ISelCtx *ctx) {
  static Reg argRegs[]
      = {RV_A0, RV_A1, RV_A2, RV_A3, RV_A4, RV_A5, RV_A6, RV_A7, REG_INVAL};

  int usedArgRegs = 0;

  RVInst *setargs = newRVInst(RV_NOP);
  rvInsertInstAfter(ctx->block->instTail->prev, setargs);

  for (int i = 0; i < arrlen(ctx->irFunc->args); i++) {
    Value *arg = ctx->irFunc->args[i];

    switch (arg->ty->kind) {
    case IR_I32:
      assert(argRegs[usedArgRegs] != REG_INVAL
             && "Argument registers are used up");

      rvInstAddReg(setargs, argRegs[usedArgRegs], REG_IMPLICIT | REG_DEFINE);

      RVInst *mv = newRVInst(RV_MV);
      rvInstAddVirtReg(mv, arg->id, REG_DEFINE);
      rvInstAddReg(mv, argRegs[usedArgRegs], REG_USE);
      rvInsertInstAfter(ctx->block->instTail->prev, mv);

      usedArgRegs++;
      break;

    default:
      assert(false);
    }
  }
}

static void iselBlock(ISelCtx *ctx, RVBlock *block, IRBlock *irBlock) {
  ctx->irBlock = irBlock;
  ctx->block = block;

  for (int j = 0; j < arrlen(irBlock->preds); j++)
    arrput(block->preds, ctx->func->blocks[irBlock->preds[j]->id]);
  for (int j = 0; j < arrlen(irBlock->succs); j++)
    arrput(block->succs, ctx->func->blocks[irBlock->succs[j]->id]);

  for (IRInst *irInst = irBlock->instHead->next; irInst != irBlock->instTail;
       irInst = irInst->next) {
    iselInst(ctx, irInst);
  }

  ctx->irBlock = NULL;
  ctx->block = NULL;
}

static void iselInst(ISelCtx *ctx, IRInst *irInst) {
  Value **srcs = irInst->srcs;
  for (int i = 0; i < arrlen(srcs); i++) {
    if (srcs[i]->kind == IR_VAL_DAG_NODE)
      iselInst(ctx, srcs[i]->inst);
  }

  switch (irInst->kind) {
  case IR_LOAD:
    return iselLoad(ctx, irInst);
  case IR_STORE:
    return iselStore(ctx, irInst);
  case IR_ADD:
    return iselAdd(ctx, irInst);
  case IR_SUB:
    return iselSub(ctx, irInst);
  case IR_LI:
    return iselLi(ctx, irInst);
  case IR_BR:
    return iselBr(ctx, irInst);
  case IR_J:
    return iselJ(ctx, irInst);
  case IR_RET:
    return iselRet(ctx, irInst);
  default:
    assert(false);
  }
}

static void iselLoad(ISelCtx *ctx, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;
  assert(srcs[0]->ty->kind == IR_PTR && "Should load from a pointer");

  switch (dst->ty->kind) {
  case IR_I32:;
    RVInst *inst = newRVInst(RV_LW);
    rvInstAddVirtReg(inst, dst->id, REG_DEFINE);
    rvInstAddFrameObj(inst, srcs[0]->id, 0);
    return rvInsertInstAfter(ctx->block->instTail->prev, inst);

  default:
    assert(false);
  }
}

static void iselStore(ISelCtx *ctx, IRInst *irInst) {
  Value **srcs = irInst->srcs;
  assert(srcs[0]->ty->kind == IR_PTR && "Should store to a pointer");

  switch (srcs[1]->ty->kind) {
  case IR_I32:;
    RVInst *inst = newRVInst(RV_SW);
    rvInstAddVirtReg(inst, srcs[1]->id, 0);
    rvInstAddFrameObj(inst, srcs[0]->id, REG_DEFINE);
    return rvInsertInstAfter(ctx->block->instTail->prev, inst);

  default:
    assert(false);
  }
}

static void iselAdd(ISelCtx *ctx, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_ADD);
  rvInstAddVirtReg(inst, dst->id, REG_DEFINE);
  rvInstAddVirtReg(inst, srcs[0]->id, REG_USE);
  rvInstAddVirtReg(inst, srcs[1]->id, REG_USE);
  rvInsertInstAfter(ctx->block->instTail->prev, inst);
}

static void iselSub(ISelCtx *ctx, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_SUB);
  rvInstAddVirtReg(inst, dst->id, REG_DEFINE);
  rvInstAddVirtReg(inst, srcs[0]->id, REG_USE);
  rvInstAddVirtReg(inst, srcs[1]->id, REG_USE);
  rvInsertInstAfter(ctx->block->instTail->prev, inst);
}

static void iselLi(ISelCtx *ctx, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_LI);
  rvInstAddVirtReg(inst, dst->id, REG_DEFINE);
  rvInstAddImm(inst, srcs[0]->imm);
  rvInsertInstAfter(ctx->block->instTail->prev, inst);
}

static void iselBr(ISelCtx *ctx, IRInst *irInst) {
  Value **srcs = irInst->srcs;

  RVInst *bnez = newRVInst(RV_BNEZ);
  rvInstAddVirtReg(bnez, srcs[0]->id, REG_USE);
  rvInstAddBlock(bnez, srcs[1]->block->id);
  rvInsertInstAfter(ctx->block->instTail->prev, bnez);

  RVInst *j = newRVInst(RV_J);
  rvInstAddBlock(j, srcs[2]->block->id);
  rvInsertInstAfter(ctx->block->instTail->prev, j);
}

static void iselJ(ISelCtx *ctx, IRInst *irInst) {
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_J);
  rvInstAddBlock(inst, srcs[0]->block->id);
  rvInsertInstAfter(ctx->block->instTail->prev, inst);
}

static void iselRet(ISelCtx *ctx, IRInst *irInst) {
  Value **srcs = irInst->srcs;

  RVInst *ret = newRVInst(RV_RET);

  if (srcs[0]->kind == IR_VAL_VOID)
    return rvInsertInstAfter(ctx->block->instTail->prev, ret);

  RVInst *mv = newRVInst(RV_MV);
  rvInstAddReg(mv, RV_A0, REG_DEFINE);
  rvInstAddVirtReg(mv, srcs[0]->id, REG_USE);
  rvInsertInstAfter(ctx->block->instTail->prev, mv);

  rvInstAddReg(ret, RV_A0, REG_IMPLICIT);
  rvInsertInstAfter(ctx->block->instTail->prev, ret);
}
