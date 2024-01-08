#include <stdbool.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/block.h"
#include "riscv/ctx.h"
#include "riscv/inst.h"
#include "riscv/objfile.h"
#include "riscv/operand.h"
#include "riscv/reg.h"
#include "util/visitord.h"

static bool liveVarAnalysisBlockBefore(RVCtx *ctx, RVBlock *block);
static Operand *liveVarAnalysisOperand(RVCtx *ctx, Operand *op);
static bool liveVarAnalysisBlockAfter(RVCtx *ctx, RVBlock *block);

static bool setKillFlagPrepareBlock(RVCtx *ctx, RVBlock *block);
static void setKillFlagInst(RVCtx *ctx, RVInst *inst);
static bool setKillFlagCleanupBlock(RVCtx *ctx, RVBlock *block);

typedef struct LiveVarAnalysisData {
  bool liveOutsChanged;
} LiveVarAnalysisData;

void liveVarAnalysis(ObjectFile *objFile) {
  RVCtx *ctx = newRVCtx(objFile);
  ctx->blockVisitor = liveVarAnalysisBlockBefore;
  ctx->operandVisitor = liveVarAnalysisOperand;
  ctx->blockVisitorAfter = liveVarAnalysisBlockAfter;
  ctx->blockVisitOrder = VISIT_ORD_REVERSE_UNTIL_UNCHANGED;
  visitObjectFile(ctx);
  free(ctx);

  ctx = newRVCtx(objFile);
  ctx->blockVisitor = setKillFlagPrepareBlock;
  ctx->instVisitor = setKillFlagInst;
  ctx->blockVisitorAfter = setKillFlagCleanupBlock;
  ctx->instVisitOrder = VISIT_ORD_REVERSE;
  visitObjectFile(ctx);
  free(ctx);
}

static bool liveVarAnalysisBlockBefore(RVCtx *ctx, RVBlock *block) {
  Reg *oldLiveOuts = block->liveOuts;

  Reg *newLiveOuts = NULL;
  for (int i = 0; i < arrlen(block->succs); i++)
    newLiveOuts = mergeRegSet(newLiveOuts, block->succs[i]->liveIns);
  block->liveOuts = newLiveOuts;

  LiveVarAnalysisData *data = calloc(1, sizeof(LiveVarAnalysisData));
  data->liveOutsChanged = !regArrIsSame(oldLiveOuts, newLiveOuts);
  ctx->data = data;

  arrfree(oldLiveOuts);
  return false;
}

static Operand *liveVarAnalysisOperand(RVCtx *ctx, Operand *op) {
  if (op->kind != RV_OP_REG)
    return op;

  if (op->regState & REG_DEFINE) {
    arrput(ctx->block->kills, op->reg);
  } else if (op->regState & REG_UNDEF) {
    // no-op
  } else {
    if (!regArrIncludeReg(ctx->block->kills, op->reg))
      arrput(ctx->block->gens, op->reg);
  }
  return op;
}

static bool liveVarAnalysisBlockAfter(RVCtx *ctx, RVBlock *block) {
  LiveVarAnalysisData *data = ctx->data;

  makeRegSet(block->gens);
  makeRegSet(block->kills);

  Reg *oldLiveIns = block->liveIns;

  Reg *liveOutExcludeKills = subtractRegSet(block->liveOuts, block->kills);
  Reg *newLiveIns = mergeRegSet(block->gens, liveOutExcludeKills);

  bool changed = data->liveOutsChanged || !regArrIsSame(oldLiveIns, newLiveIns);

  arrfree(liveOutExcludeKills);
  arrfree(oldLiveIns);
  block->liveIns = newLiveIns;
  free(data);
  return changed;
}

typedef struct {
  Reg *killeds;
} SetKillFlagCtxData;

static bool setKillFlagPrepareBlock(RVCtx *ctx, RVBlock *block) {
  ctx->data = calloc(1, sizeof(SetKillFlagCtxData));
  return false;
}

static void setKillFlagInst(RVCtx *ctx, RVInst *inst) {
  SetKillFlagCtxData *ctxData = ctx->data;

  Reg *newKilled = NULL;

  for (int i = 0; i < arrlen(inst->operands); i++) {
    if (inst->operands[i]->kind != RV_OP_REG)
      continue;

    Reg reg = inst->operands[i]->reg;
    if (regArrIncludeReg(ctx->block->liveOuts, reg)
        || regArrIncludeReg(ctxData->killeds, reg))
      continue;

    if (inst->operands[i]->regState & REG_DEFINE)
      inst->operands[i]->regState |= REG_DEAD;
    else if (inst->operands[i]->regState & REG_UNDEF)
      ; // no-op
    else
      inst->operands[i]->regState |= REG_KILL;

    arrput(newKilled, reg);
  }

  for (int i = 0; i < arrlen(newKilled); i++)
    arrput(ctxData->killeds, newKilled[i]);
}

static bool setKillFlagCleanupBlock(RVCtx *ctx, RVBlock *block) {
  arrfree(((SetKillFlagCtxData *)ctx->data)->killeds);
  free(ctx->data);
  ctx->data = NULL;
  return false;
}
