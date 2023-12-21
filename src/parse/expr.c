#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/number.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/symbol.h"
#include "parse/type.h"

static bool setExprCType(ParseCtx *ctx, Expr *expr);
static ExprKind binaryExprKindFromPunct(Punct p);
static ExprPrecedence exprPrecedence(ExprKind k);

Expr *newExpr(ExprKind kind) {
  Expr *expr = calloc(1, sizeof(Expr));
  expr->kind = kind;
  return expr;
}

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
    if (!setExprCType(ctx, val)) {
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
    assert(setExprCType(ctx, val) == true);
    arrput(valStack, val);
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
        if (!setExprCType(ctx, stackOp)) {
          printf("cannot determine type of the expression\n");
          exit(1);
        }
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
    if (!setExprCType(ctx, stackOp)) {
      printf("cannot determine type of the expression\n");
      exit(1);
    }
    arrput(valStack, stackOp);
  }

  assert(arrlen(valStack) == 1);
  *result = arrlast(valStack);
  arrfree(valStack);
  arrfree(opStack);
  return p - begin;
}

void printExpr(Expr *expr, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");

  bool printX = false, printY = false;

  switch (expr->kind) {
  case EXPR_IDENT:
    printf("Expr Ident '%s'\n", expr->ident);
    break;

  case EXPR_NUM:
    printf("Expr Number ");
    printNumber(expr->num);
    printf("\n");
    break;

  case EXPR_MUL:
    printf("Expr Mul\n");
    printX = printY = true;
    break;

  case EXPR_ADD:
    printf("Expr Add\n");
    printX = printY = true;
    break;

  case EXPR_SUB:
    printf("Expr Sub\n");
    printX = printY = true;
    break;

  case EXPR_EQ_ASSIGN:
    printf("Expr EqAssign\n");
    printX = printY = true;
    break;

  case EXPR_COMMA:
    printf("Expr Comma\n");
    printX = printY = true;
    break;

  default:
    assert(false);
  }

  printCType(expr->ty, indent + 1);
  if (printX)
    printExpr(expr->x, indent + 1);
  if (printY)
    printExpr(expr->y, indent + 1);
}

static bool setExprCType(ParseCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);
    if (sym == NULL) {
      return false;
    }
    expr->ty = sym->ty;
    return true;

  case EXPR_NUM:
    expr->ty = expr->num->ty;
    return true;

  case EXPR_MUL:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return true;
    }
    return false;

  case EXPR_ADD:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return true;
    }
    return false;

  case EXPR_SUB:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return true;
    }
    return false;

  case EXPR_EQ_ASSIGN:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = expr->x->ty;
      return true;
    }
    return false;

  case EXPR_COMMA:
    expr->ty = expr->y->ty;
    return true;

  default:
    return false;
  }
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