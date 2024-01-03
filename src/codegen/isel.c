#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/inst.h"
#include "codegen/objfile.h"
#include "ir/ir.h"

static RVFunc *iselFunc(IRFunc *irFunc);
static RVBlock *iselBlock(IRBlock *irBlock);
static RVInst *iselInst(IRInst *irInst);

static RVInst *iselLoad(IRInst *irInst, RVInst *inst);
static RVInst *iselStore(IRInst *irInst, RVInst *inst);
static RVInst *iselAdd(IRInst *irInst, RVInst *inst);
static RVInst *iselSub(IRInst *irInst, RVInst *inst);
static RVInst *iselLi(IRInst *irInst, RVInst *inst);
static RVInst *iselRet(IRInst *irInst, RVInst *inst);

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
    RVBlock *blk = iselBlock(irFunc->blocks[i]);
    arrput(func->blocks, blk);
  }

  for (int i = 1; i <= func->blockCount; i++) {
    RVBlock *blk = func->blocks[i];
    IRBlock *irBlk = irFunc->blocks[i];

    for (int j = 0; j < arrlen(irBlk->precs); j++)
      arrput(blk->precs, func->blocks[irBlk->precs[j]->id]);

    for (int j = 0; j < arrlen(irBlk->succs); j++)
      arrput(blk->succs, func->blocks[irBlk->succs[j]->id]);
  }

  func->entry = func->blocks[irFunc->entry->id];

  arrsetcap(func->exits, arrlen(irFunc->exits));
  for (int i = 0; i < arrlen(irFunc->exits); i++)
    arrput(func->exits, func->blocks[irFunc->exits[i]->id]);

  return func;
}

static RVBlock *iselBlock(IRBlock *irBlock) {
  RVBlock *blk = newRVBlock(irBlock);

  for (IRInst *irInst = irBlock->instHead->next; irInst != irBlock->instTail;
       irInst = irInst->next) {
    RVInst *inst = iselInst(irInst);
    rvBlockAddInst(blk, inst);
  }

  return blk;
}

static RVInst *iselInst(IRInst *irInst) {
  RVInst *inst = newRVInst(RV_ILLEGAL);

  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;
  for (int i = 0; i < arrlen(srcs); i++) {
    if (srcs[i]->kind == IR_VAL_DAG_NODE)
      arrput(inst->deps, iselInst(srcs[i]->inst));
  }

  switch (irInst->kind) {
  case IR_LOAD:
    return iselLoad(irInst, inst);
  case IR_STORE:
    return iselStore(irInst, inst);
  case IR_ADD:
    return iselAdd(irInst, inst);
  case IR_SUB:
    return iselSub(irInst, inst);
  case IR_LI:
    return iselLi(irInst, inst);
  case IR_RET:
    return iselRet(irInst, inst);
  default:
    assert(false);
  }
}

static RVInst *iselLoad(IRInst *irInst, RVInst *inst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;
  assert(srcs[0]->ty->kind == IR_PTR);

  switch (dst->ty->kind) {
  case IR_I32:
    inst->kind = RV_LW;
    arrput(inst->operands, newOperandVirtReg(dst->id));
    arrput(inst->operands, newOperandFrameObj(srcs[0]->id));
    return inst;

  default:
    assert(false);
  }
}

static RVInst *iselStore(IRInst *irInst, RVInst *inst) {
  Value **srcs = irInst->srcs;
  assert(srcs[0]->ty->kind == IR_PTR);

  switch (srcs[1]->ty->kind) {
  case IR_I32:
    inst->kind = RV_SW;
    arrput(inst->operands, newOperandVirtReg(srcs[1]->id));
    arrput(inst->operands, newOperandFrameObj(srcs[0]->id));
    return inst;

  default:
    assert(false);
  }
}

static RVInst *iselAdd(IRInst *irInst, RVInst *inst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  inst->kind = RV_ADD;
  arrput(inst->operands, newOperandVirtReg(dst->id));
  arrput(inst->operands, newOperandVirtReg(srcs[0]->id));
  arrput(inst->operands, newOperandVirtReg(srcs[1]->id));
  return inst;
}

static RVInst *iselSub(IRInst *irInst, RVInst *inst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  inst->kind = RV_SUB;
  arrput(inst->operands, newOperandVirtReg(dst->id));
  arrput(inst->operands, newOperandVirtReg(srcs[0]->id));
  arrput(inst->operands, newOperandVirtReg(srcs[1]->id));
  return inst;
}

static RVInst *iselLi(IRInst *irInst, RVInst *inst) {
  Value *dst = irInst->dst;
  Value **srcs = irInst->srcs;

  inst->kind = RV_LI;
  arrput(inst->operands, newOperandVirtReg(dst->id));
  arrput(inst->operands, newOperandImm(srcs[0]->imm));
  return inst;
}

static RVInst *iselRet(IRInst *irInst, RVInst *inst) {
  Value **srcs = irInst->srcs;

  inst->kind = RV_RET;

  if (srcs[0]->kind == IR_VAL_VOID)
    return inst;

  RVInst *mv = newRVInst(RV_MV);
  arrput(mv->operands, newOperandReg(RV_A0));
  arrput(mv->operands, newOperandVirtReg(srcs[0]->inst->dst->id));
  arrput(inst->deps, mv);
  return inst;
}
