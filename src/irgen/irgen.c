#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "parse/parse.h"
#include "parse/type.h"
#include "parse/decl.h"

Module *generateIR(TranslationUnit *unit) {
  Module *mod = calloc(1, sizeof(Module));

  IRGenCtx ctx;
  memset(&ctx, 0, sizeof(ctx));

  for (int i = 0; i < arrlen(unit->decltions); i++) {
    assert(unit->decltions[i]->funcDef != NULL);

    Declarator *decl = unit->decltions[i]->decltors[0];

    Func *func = newFunc(decl->ident);
    func->ret = newIRTypeFromCType(decl->ty->func.ret);

    for (int i = 0; i < arrlen(decl->ty->func.params); i++) {
      Declarator *cParam = decl->ty->func.params[i];
      Val *param = newValVar(newIRTypeFromCType(cParam->ty), cParam->ident);
      arrput(func->params, param);
    }

    arrput(mod->funcs, func);
  }

  return mod;
}
