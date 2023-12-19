#include <assert.h>
#include <stdbool.h>

#include "stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/irgen.h"
#include "parse/expr.h"
#include "parse/symbol.h"

Value *genExpr(IRGenCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);

    IRInst *load = newIRInst(IR_LOAD);
    load->dst = newValueVar(newIRTypeFromCType(sym->ty), expr->ident);
    load->src1 = sym->irValPtr;
    arrput(ctx->block->insts, load);
    return load->dst;

  default:
    assert(false);
  }
}
