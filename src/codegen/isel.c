#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/inst.h"
#include "codegen/objfile.h"
#include "ir/inst.h"
#include "ir/module.h"

static RVFunc *iselFunc(IRFunc *irFunc);
static void iselArgs(IRFunc *irFunc, RVBlock *entryBlock);
static RVBlock *iselBlock(IRBlock *irBlock, RVBlock *block);
static void iselInst(RVBlock *irBlock, IRInst *irInst);

static void iselLoad(RVBlock *irBlock, IRInst *irInst);
static void iselStore(RVBlock *irBlock, IRInst *irInst);
static void iselAdd(RVBlock *irBlock, IRInst *irInst);
static void iselSub(RVBlock *irBlock, IRInst *irInst);
static void iselLi(RVBlock *irBlock, IRInst *irInst);
static void iselRet(RVBlock *irBlock, IRInst *irInst);

ObjectFile *selectInstruction(Module *mod) {
  ObjectFile *objFile = calloc(1, sizeof(ObjectFile));

  for (int i = 0; i < arrlen(mod->funcs); i++) {
    RVFunc *func = iselFunc(mod->funcs[i]);
    arrput(objFile->funcs, func);
  }

  return objFile;
}

static RVFunc *iselFunc(IRFunc *irFunc) {
  RVFunc *func = newRVFunc(irFunc);

  for (int i = 1; i <= func->blockCount; i++) {
    RVBlock *blk = newRVBlock(irFunc->blocks[i]);

    if (blk->id == irFunc->entry->id)
      iselArgs(irFunc, blk);

    iselBlock(irFunc->blocks[i], blk);
    arrput(func->blocks, blk);
  }

  for (int i = 1; i <= func->blockCount; i++) {
    RVBlock *blk = func->blocks[i];
    IRBlock *irBlk = irFunc->blocks[i];

    for (int j = 0; j < arrlen(irBlk->preds); j++)
      arrput(blk->preds, func->blocks[irBlk->preds[j]->id]);

    for (int j = 0; j < arrlen(irBlk->succs); j++)
      arrput(blk->succs, func->blocks[irBlk->succs[j]->id]);
  }

  func->entry = func->blocks[irFunc->entry->id];

  arrsetcap(func->exits, arrlen(irFunc->exits));
  for (int i = 0; i < arrlen(irFunc->exits); i++)
    arrput(func->exits, func->blocks[irFunc->exits[i]->id]);

  return func;
}

static void iselArgs(IRFunc *irFunc, RVBlock *entryBlock) {
  static Reg argRegs[]
      = {RV_A0, RV_A1, RV_A2, RV_A3, RV_A4, RV_A5, RV_A6, RV_A7, RV_ZERO};

  int usedArgRegs = 0;

  for (int i = 0; i < arrlen(irFunc->args); i++) {
    Value *arg = irFunc->args[i];

    switch (arg->ty->kind) {
    case IR_I32:
      if (argRegs[usedArgRegs] == RV_ZERO)
        assert(false);
      RVInst *mv = newRVInst(RV_MV);
      arrput(mv->operands, newOperandVirtReg(arg->id));
      arrput(mv->operands, newOperandReg(argRegs[usedArgRegs]));
      rvBlockAddInst(entryBlock, mv);
      usedArgRegs++;
      break;

    default:
      assert(false);
    }
  }
}

static RVBlock *iselBlock(IRBlock *irBlock, RVBlock *blk) {
  for (IRInst *irInst = irBlock->instHead->next; irInst != irBlock->instTail;
       irInst = irInst->next) {
    iselInst(blk, irInst);
  }

  return blk;
}

static void iselInst(RVBlock *rvBlock, IRInst *irInst) {
  Value **srcs = irInst->srcs;
  for (int i = 0; i < arrlen(srcs); i++) {
    if (srcs[i]->kind == IR_VAL_DAG_NODE)
      iselInst(rvBlock, srcs[i]->inst);
  }

  switch (irInst->kind) {
  case IR_LOAD:
    return iselLoad(rvBlock, irInst);
  case IR_STORE:
    return iselStore(rvBlock, irInst);
  case IR_ADD:
    return iselAdd(rvBlock, irInst);
  case IR_SUB:
    return iselSub(rvBlock, irInst);
  case IR_LI:
    return iselLi(rvBlock, irInst);
  case IR_RET:
    return iselRet(rvBlock, irInst);
  default:
    assert(false);
  }
}

static void iselLoad(RVBlock *block, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;
  assert(srcs[0]->ty->kind == IR_PTR);

  switch (dst->ty->kind) {
  case IR_I32:;
    RVInst *inst = newRVInst(RV_LW);
    arrput(inst->operands, newOperandVirtReg(dst->id));
    arrput(inst->operands, newOperandFrameObj(srcs[0]->id));
    return rvBlockAddInst(block, inst);

  default:
    assert(false);
  }
}

static void iselStore(RVBlock *block, IRInst *irInst) {
  Value **srcs = irInst->srcs;
  assert(srcs[0]->ty->kind == IR_PTR);

  switch (srcs[1]->ty->kind) {
  case IR_I32:;
    RVInst *inst = newRVInst(RV_SW);
    arrput(inst->operands, newOperandVirtReg(srcs[1]->id));
    arrput(inst->operands, newOperandFrameObj(srcs[0]->id));
    return rvBlockAddInst(block, inst);

  default:
    assert(false);
  }
}

static void iselAdd(RVBlock *block, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_ADD);
  arrput(inst->operands, newOperandVirtReg(dst->id));
  arrput(inst->operands, newOperandVirtReg(srcs[0]->id));
  arrput(inst->operands, newOperandVirtReg(srcs[1]->id));
  rvBlockAddInst(block, inst);
}

static void iselSub(RVBlock *block, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_SUB);
  arrput(inst->operands, newOperandVirtReg(dst->id));
  arrput(inst->operands, newOperandVirtReg(srcs[0]->id));
  arrput(inst->operands, newOperandVirtReg(srcs[1]->id));
  rvBlockAddInst(block, inst);
}

static void iselLi(RVBlock *block, IRInst *irInst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  RVInst *inst = newRVInst(RV_LI);
  arrput(inst->operands, newOperandVirtReg(dst->id));
  arrput(inst->operands, newOperandImm(srcs[0]->imm));
  rvBlockAddInst(block, inst);
}

static void iselRet(RVBlock *block, IRInst *irInst) {
  Value **srcs = irInst->srcs;

  RVInst *ret = newRVInst(RV_RET);

  if (srcs[0]->kind == IR_VAL_VOID)
    return rvBlockAddInst(block, ret);

  RVInst *mv = newRVInst(RV_MV);
  arrput(mv->operands, newOperandReg(RV_A0));
  arrput(mv->operands, newOperandVirtReg(srcs[0]->inst->dst->id));
  rvBlockAddInst(block, mv);

  rvBlockAddInst(block, ret);
}
