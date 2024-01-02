#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

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

IRFunc *genFunc(IRGenCtx *ctx, Declaration *decl) {
  assert(decl->funcDef != NULL);

  IRFunc *func = newIRFunc(decl->decltors[0]->ident);
  func->ret = newIRTypeFromCType(decl->decltors[0]->ty->func.ret);
  func->entry = newIRBlock(func, NULL);

  ctx->cFunc = decl;
  ctx->symtab = decl->funcDef->symtab;
  ctx->irFunc = func;
  ctx->block = func->entry;

  genArgs(ctx, decl->decltors[0]->ty->func.params);

  genStmt(ctx, decl->funcDef);

  ctx->cFunc = NULL;
  ctx->symtab = NULL;
  ctx->irFunc = NULL;
  ctx->block = NULL;
  ctx->unreachable = false;
  return func;
}

static void genArgs(IRGenCtx *ctx, Declarator **params) {
  for (int i = 0; i < arrlen(params); i++) {
    IRType *ty = newIRTypeFromCType(params[i]->ty);

    Value *arg = newValueVar(ctx->irFunc, ty);
    arrput(ctx->irFunc->args, arg);

    Symbol *sym = symTableGet(ctx->symtab, params[i]->ident);
    sym->irValPtr = newValueVar(ctx->irFunc, newIRType(IR_PTR));

    IRInst *alloca = newIRInst(IR_ALLOCA);
    alloca->src1 = newValueImm(newIRType(IR_I32), sym->ty->size);
    alloca->src2 = newValueImm(newIRType(IR_I32), sym->ty->align);
    alloca->dst = sym->irValPtr;
    arrput(ctx->irFunc->allocas, alloca);

    IRInst *store = newIRInst(IR_STORE);
    store->src1 = sym->irValPtr;
    store->src2 = arg;
    arrput(ctx->block->insts, store);
  }
}
