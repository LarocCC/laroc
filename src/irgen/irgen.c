#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/module.h"
#include "irgen/func.h"
#include "irgen/irgen.h"
#include "sema/decl.h"
#include "sema/transunit.h"

Module *genIR(TranslationUnit *unit) {
  Module *mod = calloc(1, sizeof(Module));

  IRGenCtx ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.symtab = unit->symtab;

  for (int i = 0; i < arrlen(unit->decltions); i++) {
    assert(unit->decltions[i]->funcDef
           && "Don't know how to generate for top-level declarations");
    IRFunc *func = genFunc(&ctx, unit->decltions[i]);
    arrput(mod->funcs, func);
  }

  return mod;
}
