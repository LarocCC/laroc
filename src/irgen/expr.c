#include <assert.h>
#include <stdbool.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "lex/number.h"
#include "parse/expr.h"
#include "parse/symbol.h"
#include "parse/type.h"

Value *genExpr(IRGenCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);

    IRInst *load = newIRInst(IR_LOAD);
    load->src1 = sym->irValPtr;
    load->dst = newValueVar(newIRTypeFromCType(sym->ty), expr->ident);
    arrput(ctx->block->insts, load);
    return load->dst;

  case EXPR_NUM:
    assert(typeIsInteger(expr->ty));
    return newValueImm(newIRTypeFromCType(expr->ty), expr->num->x);

  case EXPR_ADD:;
    IRInst *add = newIRInst(IR_ADD);
    add->src1 = genExpr(ctx, expr->x);
    add->src2 = genExpr(ctx, expr->y);
    add->dst = newValueVar(newIRTypeFromCType(expr->ty), NULL);
    arrput(ctx->block->insts, add);
    return add->dst;

  case EXPR_SUB:;
    IRInst *sub = newIRInst(IR_SUB);
    sub->src1 = genExpr(ctx, expr->x);
    sub->src2 = genExpr(ctx, expr->y);
    sub->dst = newValueVar(newIRTypeFromCType(expr->ty), NULL);
    arrput(ctx->block->insts, sub);
    return sub->dst;

  default:
    assert(false);
  }
}
