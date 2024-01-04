#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/module.h"
#include "ir/type.h"
#include "irgen/func.h"
#include "irgen/irgen.h"
#include "parse/decl.h"
#include "parse/parse.h"
#include "parse/type.h"

Module *genIR(TranslationUnit *unit) {
  Module *mod = calloc(1, sizeof(Module));

  IRGenCtx ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.symtab = unit->symtab;

  for (int i = 0; i < arrlen(unit->decltions); i++) {
    assert(unit->decltions[i]->funcDef != NULL);
    IRFunc *func = genFunc(&ctx, unit->decltions[i]);
    arrput(mod->funcs, func);
  }

  return mod;
}

IRType *newIRTypeFromCType(CType *cTy) {
  IRType *irTy = calloc(1, sizeof(IRType));
  switch (cTy->kind) {
  case TYPE_VOID:
    irTy->kind = IR_VOID;
    break;
  case TYPE_INT:
    irTy->kind = IR_I32;
    break;
  default:
    assert(false);
  }
  return irTy;
}
