#include <assert.h>
#include <stdbool.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "ir/inst.h"
#include "ir/type.h"
#include "ir/value.h"
#include "irgen/irgen.h"
#include "irgen/type.h"
#include "lex/number.h"
#include "sema/expr.h"
#include "sema/symbol.h"
#include "sema/type.h"

static Value *genLvaluePtr(IRGenCtx *ctx, Expr *expr);

static Value *genLogicAndExpr(IRGenCtx *ctx, Expr *expr);
static Value *genLogicOrExpr(IRGenCtx *ctx, Expr *expr);
static Value *genCondExpr(IRGenCtx *ctx, Expr *expr);

Value *genExpr(IRGenCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT: {
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);

    IRInst *load = newIRInst(IR_LOAD);
    arrput(load->srcs, sym->irValPtr);
    load->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(sym->ty));
    irBlockAddInst(ctx->block, load);
    return load->dst;
  }
  case EXPR_NUM: {
    assert(typeIsInteger(expr->ty)
           && "Don't know how to generate for non-integers");
    IRInst *li = newIRInst(IR_LI);
    li->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    arrput(li->srcs, newValueImm(expr->num->i));
    irBlockAddInst(ctx->block, li);
    return li->dst;
  }

  case EXPR_MUL: {
    IRInst *mul = newIRInst(IR_MUL);
    arrput(mul->srcs, genExpr(ctx, expr->x));
    arrput(mul->srcs, genExpr(ctx, expr->y));
    mul->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, mul);
    return mul->dst;
  }
  case EXPR_DIV: {
    IRInst *div = newIRInst(IR_DIV);
    arrput(div->srcs, genExpr(ctx, expr->x));
    arrput(div->srcs, genExpr(ctx, expr->y));
    div->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, div);
    return div->dst;
  }
  case EXPR_MOD: {
    IRInst *mod = newIRInst(IR_MOD);
    arrput(mod->srcs, genExpr(ctx, expr->x));
    arrput(mod->srcs, genExpr(ctx, expr->y));
    mod->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, mod);
    return mod->dst;
  }

  case EXPR_ADD: {
    IRInst *add = newIRInst(IR_ADD);
    arrput(add->srcs, genExpr(ctx, expr->x));
    arrput(add->srcs, genExpr(ctx, expr->y));
    add->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, add);
    return add->dst;
  }
  case EXPR_SUB: {
    IRInst *sub = newIRInst(IR_SUB);
    arrput(sub->srcs, genExpr(ctx, expr->x));
    arrput(sub->srcs, genExpr(ctx, expr->y));
    sub->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, sub);
    return sub->dst;
  }

  case EXPR_SHIFT_L: {
    IRInst *shl = newIRInst(IR_SHL);
    arrput(shl->srcs, genExpr(ctx, expr->x));
    arrput(shl->srcs, genExpr(ctx, expr->y));
    shl->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, shl);
    return shl->dst;
  }
  case EXPR_SHIFT_R: {
    IRInst *shr = newIRInst(IR_SHR);
    arrput(shr->srcs, genExpr(ctx, expr->x));
    arrput(shr->srcs, genExpr(ctx, expr->y));
    shr->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, shr);
    return shr->dst;
  }

  case EXPR_LT: {
    IRInst *cmp = newIRInst(IR_CMP_LT);
    arrput(cmp->srcs, genExpr(ctx, expr->x));
    arrput(cmp->srcs, genExpr(ctx, expr->y));
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, cmp);
    return cmp->dst;
  }
  case EXPR_GT: {
    IRInst *cmp = newIRInst(IR_CMP_LT);
    arrput(cmp->srcs, genExpr(ctx, expr->y));
    arrput(cmp->srcs, genExpr(ctx, expr->x));
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, cmp);
    return cmp->dst;
  }
  case EXPR_LEQ: {
    IRInst *cmp = newIRInst(IR_CMP_LT);
    arrput(cmp->srcs, genExpr(ctx, expr->y));
    arrput(cmp->srcs, genExpr(ctx, expr->x));
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, cmp);
    IRInst *nott = newIRInst(IR_NOT);
    arrput(nott->srcs, cmp->dst);
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, nott);
    return nott->dst;
  }
  case EXPR_GEQ: {
    IRInst *cmp = newIRInst(IR_CMP_LT);
    arrput(cmp->srcs, genExpr(ctx, expr->x));
    arrput(cmp->srcs, genExpr(ctx, expr->y));
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, cmp);
    IRInst *nott = newIRInst(IR_NOT);
    arrput(nott->srcs, cmp->dst);
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, nott);
    return nott->dst;
  }

  case EXPR_EQ_CMP: {
    IRInst *cmp = newIRInst(IR_CMP_EQ);
    arrput(cmp->srcs, genExpr(ctx, expr->x));
    arrput(cmp->srcs, genExpr(ctx, expr->y));
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, cmp);
    return cmp->dst;
  }
  case EXPR_NEQ: {
    IRInst *cmp = newIRInst(IR_CMP_EQ);
    arrput(cmp->srcs, genExpr(ctx, expr->y));
    arrput(cmp->srcs, genExpr(ctx, expr->x));
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, cmp);
    IRInst *nott = newIRInst(IR_NOT);
    arrput(nott->srcs, cmp->dst);
    cmp->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, nott);
    return cmp->dst;
  }

  case EXPR_BIT_AND: {
    IRInst *and = newIRInst(IR_AND);
    arrput(and->srcs, genExpr(ctx, expr->x));
    arrput(and->srcs, genExpr(ctx, expr->y));
    and->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, and);
    return and->dst;
  }

  case EXPR_BIT_XOR: {
    IRInst *xorr = newIRInst(IR_XOR);
    arrput(xorr->srcs, genExpr(ctx, expr->x));
    arrput(xorr->srcs, genExpr(ctx, expr->y));
    xorr->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, xorr);
    return xorr->dst;
  }

  case EXPR_BIT_OR: {
    IRInst *orr = newIRInst(IR_OR);
    arrput(orr->srcs, genExpr(ctx, expr->x));
    arrput(orr->srcs, genExpr(ctx, expr->y));
    orr->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
    irBlockAddInst(ctx->block, orr);
    return orr->dst;
  }

  case EXPR_LOGIC_AND: {
    return genLogicAndExpr(ctx, expr);
  }

  case EXPR_LOGIC_OR: {
    return genLogicOrExpr(ctx, expr);
  }

  case EXPR_COND: {
    return genCondExpr(ctx, expr);
  }

  case EXPR_EQ_ASSIGN: {
    IRInst *store = newIRInst(IR_STORE);
    arrput(store->srcs, genLvaluePtr(ctx, expr->x));
    arrput(store->srcs, genExpr(ctx, expr->y));
    irBlockAddInst(ctx->block, store);
    return NULL;
  }

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

static Value *genLogicAndExpr(IRGenCtx *ctx, Expr *expr) {
  assert(expr->kind == EXPR_LOGIC_AND);

  // .blkHead (ctx->block):
  //    %1 = %lhs
  //    br %1, .blkT, .blkTail,
  // .blkT:
  //    %2 = %rhs
  //    j .blkTail
  // .blkTail:
  //    %result = phi .blkHead, 0, .blkT, %2

  IRBlock *blkHead = ctx->block;
  IRBlock *blkT = newIRBlock(ctx->irFunc);
  IRBlock *blkTail = newIRBlock(ctx->irFunc);

  arrput(blkHead->succs, blkT);
  arrput(blkHead->succs, blkTail);

  arrput(blkT->preds, blkHead);
  arrput(blkT->succs, blkTail);

  arrput(blkTail->preds, blkHead);
  arrput(blkTail->preds, blkT);

  Value *lhs = genExpr(ctx, expr->x);
  IRInst *br = newIRInst(IR_BR);
  arrput(br->srcs, lhs);
  arrput(br->srcs, newValueBlock(blkT));
  arrput(br->srcs, newValueBlock(blkTail));
  irBlockAddInst(ctx->block, br);

  ctx->block = blkT;
  Value *rhs = genExpr(ctx, expr->y);
  IRInst *j = newIRInst(IR_J);
  arrput(j->srcs, newValueBlock(blkTail));
  irBlockAddInst(ctx->block, j);

  ctx->block = blkTail;
  IRInst *phi = newIRInst(IR_PHI);
  arrput(phi->srcs, newValueBlock(blkHead));
  arrput(phi->srcs, newValueImm(0));
  arrput(phi->srcs, newValueBlock(blkT));
  arrput(phi->srcs, rhs);
  phi->dst = newValueVar(ctx->irFunc, newIRType(IR_I32));
  irBlockAddInst(ctx->block, phi);

  return phi->dst;
}

static Value *genLogicOrExpr(IRGenCtx *ctx, Expr *expr) {
  assert(expr->kind == EXPR_LOGIC_OR);

  // .blkHead (ctx->block):
  //    %1 = %lhs
  //    br %1, .blkTail, .blkF
  // .blkF:
  //    %2 = %rhs
  //    j .blkTail
  // .blkTail:
  //    %result = phi .blkHead, 1, .blkF, %2

  IRBlock *blkHead = ctx->block;
  IRBlock *blkF = newIRBlock(ctx->irFunc);
  IRBlock *blkTail = newIRBlock(ctx->irFunc);

  arrput(blkHead->succs, blkF);
  arrput(blkHead->succs, blkTail);

  arrput(blkF->preds, blkHead);
  arrput(blkF->succs, blkTail);

  arrput(blkTail->preds, blkHead);
  arrput(blkTail->preds, blkF);

  Value *lhs = genExpr(ctx, expr->x);
  IRInst *br = newIRInst(IR_BR);
  arrput(br->srcs, lhs);
  arrput(br->srcs, newValueBlock(blkTail));
  arrput(br->srcs, newValueBlock(blkF));
  irBlockAddInst(ctx->block, br);

  ctx->block = blkF;
  Value *rhs = genExpr(ctx, expr->y);
  IRInst *j = newIRInst(IR_J);
  arrput(j->srcs, newValueBlock(blkTail));
  irBlockAddInst(ctx->block, j);

  ctx->block = blkTail;
  IRInst *phi = newIRInst(IR_PHI);
  arrput(phi->srcs, newValueBlock(blkHead));
  arrput(phi->srcs, newValueImm(1));
  arrput(phi->srcs, newValueBlock(blkF));
  arrput(phi->srcs, rhs);
  phi->dst = newValueVar(ctx->irFunc, newIRType(IR_I32));
  irBlockAddInst(ctx->block, phi);

  return phi->dst;
}

static Value *genCondExpr(IRGenCtx *ctx, Expr *expr) {
  assert(expr->kind == EXPR_COND);

  IRBlock *blkT = newIRBlock(ctx->irFunc);
  IRBlock *blkF = newIRBlock(ctx->irFunc);
  IRBlock *blkTail = newIRBlock(ctx->irFunc);

  arrput(ctx->block->succs, blkT);
  arrput(ctx->block->succs, blkF);
  arrput(blkT->preds, ctx->block);
  arrput(blkF->preds, ctx->block);

  IRInst *br = newIRInst(IR_BR);
  arrput(br->srcs, genExpr(ctx, expr->x));
  arrput(br->srcs, newValueBlock(blkT));
  arrput(br->srcs, newValueBlock(blkF));
  irBlockAddInst(ctx->block, br);

  ctx->block = blkT;
  Value *valT = genExpr(ctx, expr->y);
  IRInst *j = newIRInst(IR_J);
  arrput(j->srcs, newValueBlock(blkTail));
  irBlockAddInst(blkT, j);

  ctx->block = blkF;
  Value *valF = genExpr(ctx, expr->z);
  j = newIRInst(IR_J);
  arrput(j->srcs, newValueBlock(blkTail));
  irBlockAddInst(blkF, j);

  arrput(blkT->succs, blkTail);
  arrput(blkF->succs, blkTail);
  arrput(blkTail->preds, blkT);
  arrput(blkTail->preds, blkF);

  ctx->block = blkTail;
  IRInst *phi = newIRInst(IR_PHI);
  arrput(phi->srcs, newValueBlock(blkT));
  arrput(phi->srcs, valT);
  arrput(phi->srcs, newValueBlock(blkF));
  arrput(phi->srcs, valF);
  phi->dst = newValueVar(ctx->irFunc, newIRTypeFromCType(expr->ty));
  irBlockAddInst(ctx->block, phi);

  return phi->dst;
}
