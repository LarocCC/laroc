#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lex/number.h"
#include "sema/expr.h"
#include "sema/type.h"

Expr *newExpr(ExprKind kind) {
  Expr *expr = calloc(1, sizeof(Expr));
  expr->kind = kind;
  return expr;
}

void printExpr(Expr *expr, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");

  bool printX = false, printY = false, printZ = false;

  switch (expr->kind) {
  case EXPR_IDENT:
    printf("Expr Ident '%s'\n", expr->ident);
    break;

  case EXPR_NUM:
    printf("Expr Number ");
    printNumber(expr->num);
    printf("\n");
    break;

  case EXPR_MEMBER:
    printf("Expr Member\n");
    printExpr(expr->x, indent + 1);
    for (int i = 0; i < indent + 1; i++)
      printf("  ");
    printf("Ident '%s'\n", expr->ident);
    break;

  case EXPR_POSTFIX_INCR:
    printf("Expr PostfixIncr\n");
    printX = true;
    break;

  case EXPR_POSTFIX_DECR:
    printf("Expr PostfixDecr\n");
    printX = true;
    break;

  case EXPR_PREFIX_INCR:
    printf("Expr PrefixIncr\n");
    printX = true;
    break;

  case EXPR_PREFIX_DECR:
    printf("Expr PrefixDecr\n");
    printX = true;
    break;

  case EXPR_POS:
    printf("Expr Pos\n");
    printX = true;
    break;

  case EXPR_NEG:
    printf("Expr Neg\n");
    printX = true;
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

  case EXPR_COND:
    printf("Expr Cond\n");
    printX = printY = printZ = true;
    break;

  case EXPR_EQ_ASSIGN:
    printf("Expr EqAssign\n");
    printX = printY = true;
    break;

  case EXPR_ADD_EQ:
    printf("Expr AddEq\n");
    printX = printY = true;
    break;

  case EXPR_SUB_EQ:
    printf("Expr SubEq\n");
    printX = printY = true;
    break;

  case EXPR_COMMA:
    printf("Expr Comma\n");
    printX = printY = true;
    break;

  default:
    assert(false);
  }

  if (expr->ty)
    printCType(expr->ty, indent + 1);
  if (printX)
    printExpr(expr->x, indent + 1);
  if (printY)
    printExpr(expr->y, indent + 1);
  if (printZ)
    printExpr(expr->z, indent + 1);
}
