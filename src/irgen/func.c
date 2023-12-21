#include <assert.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/func.h"
#include "irgen/irgen.h"
#include "irgen/stmt.h"
#include "parse/decl.h"
#include "parse/stmt.h"
#include "parse/symbol.h"
#include "parse/type.h"

static void genArgs(IRGenCtx *ctx, Declarator **params);

Func *genFunc(IRGenCtx *ctx, Declaration *decl) {
  assert(decl->funcDef != NULL);

  Func *func = newFunc(decl->decltors[0]->ident);
  func->ret = newIRTypeFromCType(decl->decltors[0]->ty->func.ret);
  func->entry = calloc(1, sizeof(Block));

  ctx->symtab = decl->funcDef->symtab;
  ctx->func = func;
  ctx->block = func->entry;

  genArgs(ctx, decl->decltors[0]->ty->func.params);

  genStmt(ctx, decl->funcDef);

  ctx->block = NULL;
  return func;
}

static void genArgs(IRGenCtx *ctx, Declarator **params) {
  for (int i = 0; i < arrlen(params); i++) {
    IRType *ty = newIRTypeFromCType(params[i]->ty);

    Value *arg = newValueVar(ty, params[i]->ident);
    arrput(ctx->func->args, arg);

    Symbol *sym = symTableGet(ctx->symtab, params[i]->ident);
    sym->irValPtr = newValueVar(newIRType(IR_PTR), NULL);

    IRInst *alloca = newIRInst(IR_ALLOCA);
    alloca->src1 = newValueImm(newIRType(IR_I32), sym->ty->size);
    alloca->src2 = newValueImm(newIRType(IR_I32), sym->ty->align);
    alloca->dst = sym->irValPtr;
    arrput(ctx->func->allocas, alloca);

    IRInst *store = newIRInst(IR_STORE);
    store->src1 = sym->irValPtr;
    store->src2 = arg;
    arrput(ctx->block->insts, store);
  }
}
