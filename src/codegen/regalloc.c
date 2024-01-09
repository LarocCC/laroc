#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/block.h"
#include "riscv/ctx.h"
#include "riscv/inst.h"
#include "riscv/operand.h"
#include "riscv/reg.h"

typedef struct RegAllocInfo {
  Reg physicalReg;
  Reg virtReg;
} RegAllocInfo;

typedef struct RegAllocCtxData {
  RegAllocInfo *allocMap;
  RegAllocInfo *rewriteMap;
} RegAllocCtxData;

static bool allocRegsForBlock(RVCtx *ctx, RVBlock *block);
static void allocRegsForInst(RVCtx *ctx, RVInst *inst);
static bool allocRegsForBlockCleanUp(RVCtx *ctx, RVBlock *block);

void allocRegs(ObjectFile *objFile) {
  RVCtx *ctx = newRVCtx(objFile);
  ctx->blockVisitor = allocRegsForBlock;
  ctx->instVisitor = allocRegsForInst;
  ctx->blockVisitorAfter = allocRegsForBlockCleanUp;
  visitObjectFile(ctx);
  free(ctx);
}

static bool allocRegsForBlock(RVCtx *ctx, RVBlock *block) {
  for (int i = 0; i < arrlen(block->liveIns); i++) {
    assert(!regIsVirtual(block->liveIns[i])
           && "Global register allocation is not implemented");
  }

  static RegAllocInfo allocatableRegs[] = {
      {RV_A0, REG_INVAL},     {RV_A1, REG_INVAL}, {RV_A2, REG_INVAL},
      {RV_A3, REG_INVAL},     {RV_A4, REG_INVAL}, {RV_A5, REG_INVAL},
      {RV_A6, REG_INVAL},     {RV_A7, REG_INVAL}, {RV_T0, REG_INVAL},
      {RV_T1, REG_INVAL},     {RV_T2, REG_INVAL}, {RV_T3, REG_INVAL},
      {RV_T4, REG_INVAL},     {RV_T5, REG_INVAL}, {RV_T6, REG_INVAL},
      {REG_INVAL, REG_INVAL},
  };

  RegAllocCtxData *data = calloc(1, sizeof(RegAllocCtxData));
  data->allocMap = allocatableRegs;
  ctx->data = data;

  return false;
}

static Reg allocMapFind(RegAllocInfo *allocMap, Reg r) {
  if (!regIsVirtual(r))
    return r;

  for (int i = 0;; i++) {
    if (allocMap[i].physicalReg == REG_INVAL)
      break;
    if (allocMap[i].virtReg != r)
      continue;
    return allocMap[i].physicalReg;
  }
  assert(false && "Register is not allocated");
}

static Reg allocMapAdd(RegAllocInfo *allocMap, Reg r) {
  if (regIsPhysical(r)) {
    for (int i = 0;; i++) {
      if (allocMap[i].physicalReg == REG_INVAL)
        break;
      if (allocMap[i].physicalReg != r)
        continue;

      assert(allocMap[i].virtReg == REG_INVAL
             && "Register remapping is not implemented");
      allocMap[i].virtReg = r;
      return r;
    }
    assert(false && "Register is unallocatable");
  }

  assert(regIsVirtual(r) && "Cannot allocate for non-register");
  for (int i = 0;; i++) {
    if (allocMap[i].physicalReg == REG_INVAL)
      break;
    if (allocMap[i].virtReg != REG_INVAL)
      continue;

    allocMap[i].virtReg = r;
    return allocMap[i].physicalReg;
  }
  assert(false && "Register spill is not implemented");
}

static void allocMapRemove(RegAllocInfo *allocMap, Reg r) {
  if (regIsPhysical(r)) {
    for (int i = 0;; i++) {
      if (allocMap[i].physicalReg == REG_INVAL)
        break;
      if (allocMap[i].physicalReg != r)
        continue;

      assert(allocMap[i].virtReg != REG_INVAL && "Register is not allocated");
      allocMap[i].virtReg = REG_INVAL;
      return;
    }
    assert(false && "Not an allocatable register");
  }

  assert(regIsVirtual(r) && "Cannot allocate for non-register");
  for (int i = 0;; i++) {
    if (allocMap[i].physicalReg == REG_INVAL)
      break;
    if (allocMap[i].virtReg != r)
      continue;

    allocMap[i].virtReg = REG_INVAL;
    return;
  }
  assert(false && "Register is not allocated");
}

static void allocRegsForInst(RVCtx *ctx, RVInst *inst) {
  RegAllocCtxData *data = ctx->data;

  for (int i = 0; i < arrlen(inst->operands); i++) {
    Operand *op = inst->operands[i];
    if (op->kind != RV_OP_REG
        || !(regIsPhysical(op->reg) || regIsVirtual(op->reg)))
      continue;

    if (!(op->regState & REG_DEFINE)) {
      op->reg = allocMapFind(data->allocMap, op->reg);
    }

    if (op->regState & REG_KILL) {
      allocMapRemove(data->allocMap, op->reg);
    }
  }

  for (int i = 0; i < arrlen(inst->operands); i++) {
    Operand *op = inst->operands[i];
    if (op->kind != RV_OP_REG
        || !(regIsPhysical(op->reg) || regIsVirtual(op->reg)))
      continue;

    if (op->regState & REG_DEFINE) {
      Reg vReg = op->reg;
      op->reg = allocMapAdd(data->allocMap, op->reg);

      if (op->regState & REG_DEAD) {
        allocMapRemove(data->allocMap, vReg);
      }
    }
  }
}

static bool allocRegsForBlockCleanUp(RVCtx *ctx, RVBlock *block) {
  RegAllocCtxData *data = ctx->data;

  for (int i = 0;; i++) {
    if (data->allocMap[i].physicalReg == REG_INVAL)
      break;

    assert(data->allocMap[i].virtReg == REG_INVAL
           && "Register is still alive at block end");
  }
  free(data);
  ctx->data = NULL;
  return false;
}
