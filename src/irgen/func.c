#include <assert.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/func.h"
#include "irgen/irgen.h"
#include "parse/decl.h"
#include "parse/stmt.h"
#include "parse/symbol.h"
#include "parse/type.h"

Func *newFunc(const char *name) {
  Func *func = calloc(1, sizeof(Func));
  func->name = name;
  return func;
}

static void generateArgs(IRGenCtx *ctx, Declarator **params);

Func *generateFunc(IRGenCtx *ctx, Declaration *decl) {
  assert(decl->funcDef != NULL);

  Func *func = newFunc(decl->decltors[0]->ident);
  func->ret = newIRTypeFromCType(decl->decltors[0]->ty->func.ret);
  func->entry = calloc(1, sizeof(Block));

  ctx->symtab = decl->funcDef->symtab;
  ctx->func = func;
  ctx->block = func->entry;

  generateArgs(ctx, decl->decltors[0]->ty->func.params);

  ctx->block = NULL;
  return func;
}

static void generateArgs(IRGenCtx *ctx, Declarator **params) {
  for (int i = 0; i < arrlen(params); i++) {
    IRType *ty = newIRTypeFromCType(params[i]->ty);

    Val *arg = newValVar(ty, params[i]->ident);
    arrput(ctx->func->args, arg);

    Symbol *sym = symTableGet(ctx->symtab, params[i]->ident);
    sym->irValPtr = newValVar(newIRType(IR_PTR), params[i]->ident);

    IRInst *alloca = newIRInst(IR_ALLOCA);
    alloca->ty = ty;
    alloca->dst = sym->irValPtr;
    arrput(ctx->func->allocas, alloca);

    IRInst *store = newIRInst(IR_STORE);
    store->src1 = arg;
    store->dst = sym->irValPtr;
    arrput(ctx->block->insts, store);
  }
}
