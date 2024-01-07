#include <assert.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/func.h"
#include "ir/inst.h"
#include "ir/type.h"
#include "ir/value.h"
#include "irgen/decl.h"
#include "irgen/expr.h"
#include "irgen/irgen.h"
#include "sema/decl.h"
#include "sema/symbol.h"
#include "sema/type.h"

void genDeclaration(IRGenCtx *ctx, Declaration *decltion) {
  assert(!decltion->funcDef
         && "Use genFunc() for generating function defination");

  for (int i = 0; i < arrlen(decltion->decltors); i++)
    genDeclarator(ctx, decltion->decltors[i]);
}

void genDeclarator(IRGenCtx *ctx, Declarator *decltor) {
  Symbol *sym = symTableGet(ctx->symtab, decltor->ident);
  sym->irValPtr = newValueVar(ctx->irFunc, newIRType(IR_PTR));

  IRInst *alloca = newIRInst(IR_ALLOCA);
  arrput(alloca->srcs, newValueImm(sym->ty->size));
  arrput(alloca->srcs, newValueImm(sym->ty->align));
  alloca->dst = sym->irValPtr;
  arrput(ctx->irFunc->allocas, alloca);

  if (!decltor->init)
    return;

  IRInst *store = newIRInst(IR_STORE);
  arrput(store->srcs, sym->irValPtr);
  arrput(store->srcs, genExpr(ctx, decltor->init));
  irBlockAddInst(ctx->block, store);
}
