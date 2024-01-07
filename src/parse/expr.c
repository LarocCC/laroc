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

static ExprKind binaryExprKindFromPunct(Punct p);
static ExprPrecedence exprPrecedence(ExprKind k);

int parseExpr(ParseCtx *ctx, const Token *begin, ExprPrecedence maxPrecedence,
              Expr **result) {
  const Token *p = begin;

  Expr **valStack = NULL;
  Expr **opStack = NULL;
  bool expectVal = true;

parse_expression_begin:
  if (expectVal && p->kind == TOK_IDENT) {
    Expr *val = newExpr(EXPR_IDENT);
    val->ident = p->ident;
    p++;
    if (!symTableGet(ctx->symtab, val->ident)) {
      printf("cannot determine the type of %s\n", val->ident);
      exit(1);
    }
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  if (expectVal && p->kind == TOK_NUM) {
    Expr *val = newExpr(EXPR_NUM);
    val->num = p->num;
    p++;
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  if (expectVal && tokenIsPunct(p, PUNCT_PAREN_L)) {
    p++;

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

  if (!expectVal && p->kind == TOK_PUNCT) {
    ExprKind binaryExprKind = binaryExprKindFromPunct(p->punct);
    if (binaryExprKind != EXPR_INVAL) {
      ExprPrecedence opPrec = exprPrecedence(binaryExprKind);
      if (opPrec > maxPrecedence)
        goto parse_expression_end;
      p++;
      Expr *op = newExpr(binaryExprKind);

      while (arrlen(opStack) > 0) {
        ExprPrecedence stackOpPerc = exprPrecedence(arrlast(opStack)->kind);
        if (opPrec < stackOpPerc)
          break;

        Expr *stackOp = arrpop(opStack);
        stackOp->y = arrpop(valStack);
        stackOp->x = arrpop(valStack);
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

  while (arrlen(opStack) > 0) {
    Expr *stackOp = arrpop(opStack);
    stackOp->y = arrpop(valStack);
    stackOp->x = arrpop(valStack);
    arrput(valStack, stackOp);
  }

  assert(arrlen(valStack) == 1);
  *result = arrlast(valStack);
  arrfree(valStack);
  arrfree(opStack);
  return p - begin;
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
