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

static void liveVarAnalysisBlockBefore(RVCtx *ctx, RVBlock *block);
static Operand *liveVarAnalysisOperand(RVCtx *ctx, Operand *op);
static void liveVarAnalysisBlockAfter(RVCtx *ctx, RVBlock *block);

static void setKillFlagPrepareBlock(RVCtx *ctx, RVBlock *block);
static void setKillFlagInst(RVCtx *ctx, RVInst *inst);
static void setKillFlagCleanupBlock(RVCtx *ctx, RVBlock *block);

void liveVarAnalysis(ObjectFile *objFile) {
  RVCtx *ctx = newRVCtx(objFile);
  ctx->blockVisitor = liveVarAnalysisBlockBefore;
  ctx->operandVisitor = liveVarAnalysisOperand;
  ctx->blockVisitorAfter = liveVarAnalysisBlockAfter;
  ctx->blockVisitOrder = VISIT_ORD_REVERSE;
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

static void liveVarAnalysisBlockBefore(RVCtx *ctx, RVBlock *block) {
  for (int i = 0; i < arrlen(block->succs); i++) {
    Reg *newLiveOuts = mergeRegArr(block->liveOuts, block->succs[i]->liveIns);
    arrfree(block->liveOuts);
    block->liveOuts = newLiveOuts;
  }
}

static Operand *liveVarAnalysisOperand(RVCtx *ctx, Operand *op) {
  if (op->kind != RV_OP_REG)
    return op;

  if (op->regState & REG_DEFINE) {
    arrput(ctx->block->kills, op->reg);
  } else {
    if (!regArrIncludeReg(ctx->block->kills, op->reg))
      arrput(ctx->block->gens, op->reg);
  }
  return op;
}

static void liveVarAnalysisBlockAfter(RVCtx *ctx, RVBlock *block) {
  sortRegArr(block->gens);
  sortRegArr(block->kills);

  Reg *liveOutExcludeKills = subtractRegArr(block->liveOuts, block->kills);
  Reg *newLiveIns = mergeRegArr(block->gens, liveOutExcludeKills);

  arrfree(liveOutExcludeKills);
  arrfree(block->liveIns);
  block->liveIns = newLiveIns;
}

typedef struct {
  Reg *killeds;
} SetKillFlagCtxData;

static void setKillFlagPrepareBlock(RVCtx *ctx, RVBlock *block) {
  ctx->data = calloc(1, sizeof(SetKillFlagCtxData));
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
    else
      inst->operands[i]->regState |= REG_KILL;

    arrput(newKilled, reg);
  }

  for (int i = 0; i < arrlen(newKilled); i++)
    arrput(ctxData->killeds, newKilled[i]);
}

static void setKillFlagCleanupBlock(RVCtx *ctx, RVBlock *block) {
  arrfree(((SetKillFlagCtxData *)ctx->data)->killeds);
  free(ctx->data);
  ctx->data = NULL;
}
