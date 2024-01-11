#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/block.h"
#include "riscv/ctx.h"
#include "riscv/frameobj.h"
#include "riscv/func.h"
#include "riscv/inst.h"
#include "riscv/objfile.h"
#include "riscv/operand.h"
#include "riscv/reg.h"
#include "util/align.h"

// +----------------------+ <--- s0, incoming sp
// |  Saved RA (8 Bytes)  |  |
// +----------------------+  |
// |  Saved S0 (8 Bytes)  |  |
// +----------------------+  |
// |  Local variables     |  |
// +----------------------+  | frameSize
// |  Padding             |  |
// +----------------------+ <--- sp

typedef struct PEICtxData {
  int frameSize;
} PEICtxData;

static void insertPrologue(RVCtx *ctx, RVFunc *func);
static void insertEpilogue(RVCtx *ctx, RVFunc *func);
static Operand *replaceOperand(RVCtx *ctx, Operand *op);

void insertPrologueEpilogue(ObjectFile *objFile) {
  RVCtx *ctx = newRVCtx(objFile);
  ctx->funcVisitor = insertPrologue;
  ctx->operandVisitor = replaceOperand;
  ctx->funcVisitorAfter = insertEpilogue;
  ctx->data = calloc(1, sizeof(PEICtxData));
  visitObjectFile(ctx);
  free(ctx);
}

static void insertPrologue(RVCtx *ctx, RVFunc *func) {
  int frameSize = 16; // 8 bytes for ra and 8 bytes for s0

  for (int i = 0; i < arrlen(func->frameObjs); i++) {
    FrameObject *o = func->frameObjs[i];

    frameSize = alignTo(frameSize + o->size, o->align);
    o->offset = frameSize;
  }
  frameSize = alignTo(frameSize, 16);

  PEICtxData *data = ctx->data;
  data->frameSize = alignTo(frameSize, 16);

  // addi sp, sp, -frameSize
  RVInst *addsp = newRVInst(RV_ADDI);
  rvInstAddReg(addsp, RV_SP, 0);
  rvInstAddReg(addsp, RV_SP, 0);
  rvInstAddImm(addsp, -frameSize);
  rvInsertInstAfter(func->entry->instHead, addsp);

  // sd ra, (frameSize - 8)(sp)
  RVInst *sdra = newRVInst(RV_SD);
  rvInstAddReg(sdra, RV_RA, 0);
  rvInstAddMem(sdra, RV_SP, frameSize - 8);
  rvInsertInstAfter(addsp, sdra);

  // sd s0, (frameSize - 16)(sp)
  RVInst *sds0 = newRVInst(RV_SD);
  rvInstAddReg(sds0, RV_S0, 0);
  rvInstAddMem(sds0, RV_SP, frameSize - 16);
  rvInsertInstAfter(sdra, sds0);

  // addi s0, sp, frameSize
  RVInst *adds0 = newRVInst(RV_ADDI);
  rvInstAddReg(adds0, RV_S0, 0);
  rvInstAddReg(adds0, RV_SP, 0);
  rvInstAddImm(adds0, frameSize);
  rvInsertInstAfter(sds0, adds0);
}

static void insertEpilogue(RVCtx *ctx, RVFunc *func) {
  PEICtxData *data = ctx->data;

  for (int i = 0; i < arrlen(func->exits); i++) {
    RVBlock *exit = func->exits[i];

    assert(exit->instTail->prev->kind == RV_RET);

    RVInst *pos = exit->instTail->prev->prev;

    // ld ra, (frameSize - 8)(sp)
    RVInst *ldra = newRVInst(RV_LD);
    rvInstAddReg(ldra, RV_RA, 0);
    rvInstAddMem(ldra, RV_SP, data->frameSize - 8);
    rvInsertInstAfter(pos, ldra);

    // ld s0, (frameSize - 16)(sp)
    RVInst *lds0 = newRVInst(RV_LD);
    rvInstAddReg(lds0, RV_S0, 0);
    rvInstAddMem(lds0, RV_SP, data->frameSize - 16);
    rvInsertInstAfter(ldra, lds0);

    // addi sp, sp, frameSize
    RVInst *addsp = newRVInst(RV_ADDI);
    rvInstAddReg(addsp, RV_SP, 0);
    rvInstAddReg(addsp, RV_SP, 0);
    rvInstAddImm(addsp, data->frameSize);
    rvInsertInstAfter(lds0, addsp);
  }
}

static Operand *replaceOperand(RVCtx *ctx, Operand *op) {
  if (op->kind != RV_OP_REG || !regIsFrameObject(op->reg))
    return op;

  bool found = false;
  int offset;
  for (int i = 0; i < arrlen(ctx->func->frameObjs); i++) {
    FrameObject *o = ctx->func->frameObjs[i];
    if (o->id == (int)op->reg - FRAME_OBJ_ID_BEGIN) {
      found = true;
      offset = -o->offset;
      break;
    }
  }
  assert(found && "Frame object does not exist");

  // replace operand with offset(s0)
  op->kind = RV_OP_MEM;
  op->reg = RV_S0;
  op->imm = offset;
  return op;
}
