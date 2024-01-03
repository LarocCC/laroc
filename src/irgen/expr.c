#include <assert.h>
#include <stdbool.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "lex/number.h"
#include "parse/expr.h"
#include "parse/symbol.h"
#include "parse/type.h"

static Value *genLvaluePtr(IRGenCtx *ctx, Expr *expr);

Value *genExpr(IRGenCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);

    IRInst *load = newIRInst(IR_LOAD);
    arrput(load->srcs, sym->irValPtr);
    load->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(sym->ty));
    irBlockAddInst(ctx->block, load);
    return load->dst;

  case EXPR_NUM:
    assert(typeIsInteger(expr->ty));
    return newValueImm(newIRTypeFromCType(expr->ty), expr->num->x);

  case EXPR_ADD:;
    IRInst *add = newIRInst(IR_ADD);
    arrput(add->srcs, genExpr(ctx, expr->x));
    arrput(add->srcs, genExpr(ctx, expr->y));
    add->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, add);
    return add->dst;

  case EXPR_SUB:;
    IRInst *sub = newIRInst(IR_SUB);
    arrput(sub->srcs, genExpr(ctx, expr->x));
    arrput(sub->srcs, genExpr(ctx, expr->y));
    sub->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, sub);
    return sub->dst;

  case EXPR_EQ_ASSIGN:;
    IRInst *store = newIRInst(IR_STORE);
    arrput(store->srcs, genLvaluePtr(ctx, expr->x));
    arrput(store->srcs, genExpr(ctx, expr->y));
    irBlockAddInst(ctx->block, store);
    return NULL;

  default:
    assert(false);
  }
}

static Value *genLvaluePtr(IRGenCtx *ctx, Expr *expr) {
  assert(expr->ty->attr & TYPE_ATTR_LVALUE);

  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);
    return sym->irValPtr;

  default:
    assert(false);
  }
}
