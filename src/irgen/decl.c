#include <assert.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/decl.h"
#include "irgen/expr.h"
#include "irgen/irgen.h"
#include "parse/decl.h"
#include "parse/symbol.h"

void genDeclaration(IRGenCtx *ctx, Declaration *decltion) {
  assert(decltion->funcDef == NULL);

  for (int i = 0; i < arrlen(decltion->decltors); i++)
    genDeclarator(ctx, decltion->decltors[i]);
}

void genDeclarator(IRGenCtx *ctx, Declarator *decltor) {
  Symbol *sym = symTableGet(ctx->symtab, decltor->ident);
  sym->irValPtr = newValueVar(newIRType(IR_PTR), NULL);

  IRInst *alloca = newIRInst(IR_ALLOCA);
  alloca->ty = newIRTypeFromCType(sym->ty);
  alloca->dst = sym->irValPtr;
  arrput(ctx->func->allocas, alloca);

  if (decltor->init == NULL)
    return;

  IRInst *store = newIRInst(IR_STORE);
  store->src1 = sym->irValPtr;
  store->src2 = genExpr(ctx, decltor->init);
  arrput(ctx->block->insts, store);
}
