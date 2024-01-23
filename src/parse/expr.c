#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/number.h"
#include "lex/token.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "sema/expr.h"
#include "sema/symbol.h"
#include "sema/type.h"

static void setExprCType(ParseCtx *ctx, Expr *expr);
static ExprKind binaryExprKindFromPunct(Punct p);
static ExprPrecedence exprPrecedence(ExprKind k);

int parseExpr(ParseCtx *ctx, const Token *begin, ExprPrecedence maxPrecedence,
              Expr **result) {
  const Token *p = begin;

  // parseExpr() uses shunting yard algorithm to build expression of values,
  // unary operators and binary operators, while other expression including
  // `(x)`, postfix expression (e.g `x[y]`, `x.ident`), `sizeof(T)`, cast
  // expression `T(x)` and conditional expression `x?y:z` are handled recursive
  // descent.
  //
  // Reference: https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm

  // Type checking of expressions is also done in this function.

  // The operand stack of shunting yard algorithm. Each element in the stack is
  // (Expr *) and should point to a valid, or completed expression.
  Expr **valStack = NULL;

  // The operator stack of shunting yard algorithm. Each element in the stack is
  // a (Expr *). The kind of each element is set but operands (x, y) are still
  // NULL.
  Expr **opStack = NULL;

  // At the beginning of parsing, or if we have just seen a binary operator,
  // expectVal is set to true indicating we are expecting next value. Otherwise,
  // if we have just seen a value, expectVal is set to false indicating we are
  // expecting the next binary operator.
  bool expectVal = true;

  // Begin of the loop
parse_expression_begin:

  // TODO: postfix-expression x[y] x(...) x.ident x->ident x++ x--

  // Primary expression

  // ident
  if (expectVal && p->kind == TOK_IDENT) {
    Expr *val = newExpr(EXPR_IDENT);
    val->ident = p->ident;
    p++;
    setExprCType(ctx, val);
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  // num
  if (expectVal && p->kind == TOK_NUM) {
    Expr *val = newExpr(EXPR_NUM);
    val->num = p->num;
    p++;
    setExprCType(ctx, val);
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  // (x)
  if (expectVal && tokenIsPunct(p, PUNCT_PAREN_L)) {
    p++;

    // TODO: (T){...}
    // TODO: (T)x

    Expr *val = newExpr(EXPR_INVAL);
    p += parseExpr(ctx, p, EXPR_PREC_ALL, &val);
    arrput(valStack, val);

    if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
      printf("expect )\n");
      exit(1);
    }
    p++;

    expectVal = false;
    goto parse_expression_begin;
  }

  // TODO: unary-expression

  // TODO: x?y:z

  // Binary expressions
  if (!expectVal && p->kind == TOK_PUNCT) {
    // Is there a binary operation matching the current punctator?
    ExprKind binaryExprKind = binaryExprKindFromPunct(p->punct);
    if (binaryExprKind != EXPR_INVAL) {
      // Is the binary operation valid under the constraint from parameter
      // maxPrecedence? Stop parsing if not.
      ExprPrecedence opPrec = exprPrecedence(binaryExprKind);
      if (opPrec > maxPrecedence)
        goto parse_expression_end;
      p++;

      Expr *op = newExpr(binaryExprKind);

      // Pop the operator stack until operator precedence is correct.
      while (arrlen(opStack) > 0) {
        ExprPrecedence stackOpPerc = exprPrecedence(arrlast(opStack)->kind);
        if (opPrec < stackOpPerc)
          break;

        Expr *stackOp = arrpop(opStack);
        stackOp->y = arrpop(valStack);
        stackOp->x = arrpop(valStack);
        setExprCType(ctx, stackOp);
        arrput(valStack, stackOp);
      }

      arrput(opStack, op);
      expectVal = true;
      goto parse_expression_begin;
    }
  }

parse_expression_end:
  if (expectVal) {
    printf("expect expression\n");
    exit(1);
  }

  // Clean the operator stack.
  while (arrlen(opStack) > 0) {
    Expr *stackOp = arrpop(opStack);
    stackOp->y = arrpop(valStack);
    stackOp->x = arrpop(valStack);
    setExprCType(ctx, stackOp);
    arrput(valStack, stackOp);
  }

  assert(arrlen(valStack) == 1);
  *result = arrlast(valStack);
  arrfree(valStack);
  arrfree(opStack);
  return p - begin;
}

static void setExprCType(ParseCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);
    if (!sym) {
      printf("cannot determine the type of %s\n", expr->ident);
      exit(1);
    }
    expr->ty = sym->ty;
    return;

  case EXPR_NUM:
    expr->ty = expr->num->ty;
    return;

  case EXPR_MUL:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return;
    }
    break;

  case EXPR_ADD:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return;
    }
    break;

  case EXPR_SUB:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return;
    }
    break;

  case EXPR_EQ_ASSIGN:
    if (!typeIsModifiableLvalue(expr->x->ty)) {
      printf("expression is not assignable\n");
      exit(1);
    }
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = expr->x->ty;
      return;
    }
    break;

  case EXPR_COMMA:
    expr->ty = expr->y->ty;
    return;

  default:
    break;
  }

  printf("cannot determine type of the expression\n");
  exit(1);
}

static ExprKind binaryExprKindFromPunct(Punct p) {
  switch (p) {
  case PUNCT_STAR:
    return EXPR_MUL;
  case PUNCT_DIV:
    return EXPR_DIV;
  case PUNCT_MOD:
    return EXPR_MOD;

  case PUNCT_PLUS:
    return EXPR_ADD;
  case PUNCT_MINUS:
    return EXPR_SUB;

  case PUNCT_SHIFT_L:
    return EXPR_SHIFT_L;
  case PUNCT_SHIFT_R:
    return EXPR_SHIFT_R;

  case PUNCT_LT:
    return EXPR_LT;
  case PUNCT_GT:
    return EXPR_GT;
  case PUNCT_LEQ:
    return EXPR_LEQ;
  case PUNCT_GEQ:
    return EXPR_GEQ;

  case PUNCT_EQ_CMP:
    return EXPR_EQ_CMP;
  case PUNCT_NEQ:
    return EXPR_NEQ;

  case PUNCT_AMP:
    return EXPR_BIT_AND;

  case PUNCT_BIT_XOR:
    return EXPR_BIT_XOR;

  case PUNCT_BIT_OR:
    return EXPR_BIT_OR;

  case PUNCT_LOGIC_AND:
    return EXPR_LOGIC_AND;

  case PUNCT_LOGIC_OR:
    return EXPR_LOGIC_OR;

  case PUNCT_EQ_ASSIGN:
    return EXPR_EQ_ASSIGN;
  case PUNCT_MUL_EQ:
    return EXPR_MUL_EQ;
  case PUNCT_DIV_EQ:
    return EXPR_DIV_EQ;
  case PUNCT_MOD_EQ:
    return EXPR_MOD_EQ;
  case PUNCT_ADD_EQ:
    return EXPR_ADD_EQ;
  case PUNCT_SUB_EQ:
    return EXPR_SUB_EQ;
  case PUNCT_SHIFT_L_EQ:
    return EXPR_SHIFT_L_EQ;
  case PUNCT_SHIFT_R_EQ:
    return EXPR_SHIFT_R_EQ;
  case PUNCT_BIT_AND_EQ:
    return EXPR_BIT_AND_EQ;
  case PUNCT_BIT_XOR_EQ:
    return EXPR_BIT_XOR_EQ;
  case PUNCT_BIT_OR_EQ:
    return EXPR_BIT_OR_EQ;

  case PUNCT_COMMA:
    return EXPR_COMMA;

  default:
    return EXPR_INVAL;
  }
}

static ExprPrecedence exprPrecedence(ExprKind k) {
  switch (k) {
  case EXPR_MUL:
  case EXPR_DIV:
  case EXPR_MOD:
    return EXPR_PREC_MUL;

  case EXPR_ADD:
  case EXPR_SUB:
    return EXPR_PREC_ADD;

  case EXPR_SHIFT_L:
  case EXPR_SHIFT_R:
    return EXPR_PREC_SHIFT;

  case EXPR_LT:
  case EXPR_GT:
  case EXPR_LEQ:
  case EXPR_GEQ:
    return EXPR_PREC_REL;

  case EXPR_EQ_CMP:
  case EXPR_NEQ:
    return EXPR_PREC_EQ;

  case EXPR_BIT_AND:
    return EXPR_PREC_BIT_AND;

  case EXPR_BIT_XOR:
    return EXPR_PREC_BIT_XOR;

  case EXPR_BIT_OR:
    return EXPR_PREC_BIT_OR;

  case EXPR_LOGIC_AND:
    return EXPR_PREC_LOGIC_AND;

  case EXPR_LOGIC_OR:
    return EXPR_PREC_LOGIC_OR;

  case EXPR_EQ_ASSIGN:
  case EXPR_MUL_EQ:
  case EXPR_DIV_EQ:
  case EXPR_MOD_EQ:
  case EXPR_ADD_EQ:
  case EXPR_SUB_EQ:
  case EXPR_SHIFT_L_EQ:
  case EXPR_SHIFT_R_EQ:
  case EXPR_BIT_AND_EQ:
  case EXPR_BIT_XOR_EQ:
  case EXPR_BIT_OR_EQ:
    return EXPR_PREC_ASSIGN;

  case EXPR_COMMA:
    return EXPR_PREC_ALL;

  default:
    return 0;
  }
}
