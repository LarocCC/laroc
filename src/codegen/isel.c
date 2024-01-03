#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/objfile.h"
#include "ir/ir.h"

static RVFunc *iselFunc(IRFunc *irFunc);
static RVBlock *iselBlock(IRBlock *irBlock);

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

  return blk;
}
