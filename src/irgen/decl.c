#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/decl.h"
#include "irgen/expr.h"
#include "irgen/irgen.h"
#include "parse/decl.h"
#include "parse/symbol.h"
#include "parse/type.h"

void genDeclaration(IRGenCtx *ctx, Declaration *decltion) {
  assert(decltion->funcDef == NULL);

  for (int i = 0; i < arrlen(decltion->decltors); i++)
    genDeclarator(ctx, decltion->decltors[i]);
}

void genDeclarator(IRGenCtx *ctx, Declarator *decltor) {
  Symbol *sym = symTableGet(ctx->symtab, decltor->ident);
  sym->irValPtr = newValueVar(ctx->irFunc, newIRType(IR_PTR));

  IRInst *alloca = newIRInst(IR_ALLOCA);
  alloca->src1 = newValueImm(newIRType(IR_I32), sym->ty->size);
  alloca->src2 = newValueImm(newIRType(IR_I32), sym->ty->align);
  alloca->dst = sym->irValPtr;
  arrput(ctx->irFunc->allocas, alloca);

  if (decltor->init == NULL)
    return;

  IRInst *store = newIRInst(IR_STORE);
  store->src1 = sym->irValPtr;
  store->src2 = genExpr(ctx, decltor->init);
  arrput(ctx->block->insts, store);
}
