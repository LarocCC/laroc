#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "lex/number.h"
#include "sema/expr.h"
#include "sema/type.h"
#include "util/diag.h"

Expr *newExpr(ExprKind kind, SourceLoc *loc) {
  Expr *expr = calloc(1, sizeof(Expr));
  expr->kind = kind;
  expr->loc = loc;
  return expr;
}

void printExpr(Expr *expr, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Expr [%d:%d] ", expr->loc->lineno, expr->loc->col);

  bool printX = false, printY = false, printZ = false;

  switch (expr->kind) {
  case EXPR_IDENT:
    printf("Ident '%s'\n", expr->ident);
    break;

  case EXPR_NUM:
    printf("Number ");
    printNumber(expr->num);
    printf("\n");
    break;

  case EXPR_MEMBER:
    printf("Member\n");
    printExpr(expr->x, indent + 1);
    for (int i = 0; i < indent + 1; i++)
      printf("  ");
    printf("Ident '%s'\n", expr->ident);
    break;

  case EXPR_POSTFIX_INCR:
    printf("PostfixIncr\n");
    printX = true;
    break;

  case EXPR_POSTFIX_DECR:
    printf("PostfixDecr\n");
    printX = true;
    break;

  case EXPR_PREFIX_INCR:
    printf("PrefixIncr\n");
    printX = true;
    break;

  case EXPR_PREFIX_DECR:
    printf("PrefixDecr\n");
    printX = true;
    break;

  case EXPR_POS:
    printf("Pos\n");
    printX = true;
    break;

  case EXPR_NEG:
    printf("Neg\n");
    printX = true;
    break;

  case EXPR_MUL:
    printf("Mul\n");
    printX = printY = true;
    break;

  case EXPR_ADD:
    printf("Add\n");
    printX = printY = true;
    break;

  case EXPR_SUB:
    printf("Sub\n");
    printX = printY = true;
    break;

  case EXPR_COND:
    printf("Cond\n");
    printX = printY = printZ = true;
    break;

  case EXPR_EQ_ASSIGN:
    printf("EqAssign\n");
    printX = printY = true;
    break;

  case EXPR_ADD_EQ:
    printf("AddEq\n");
    printX = printY = true;
    break;

  case EXPR_SUB_EQ:
    printf("SubEq\n");
    printX = printY = true;
    break;

  case EXPR_COMMA:
    printf("Comma\n");
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
