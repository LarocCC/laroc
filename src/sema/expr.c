#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/number.h"
#include "sema/expr.h"
#include "sema/number.h"
#include "sema/type.h"
#include "util/diag.h"

Expr *newExpr(ExprKind kind, SourceLoc *loc) {
  Expr *expr = calloc(1, sizeof(Expr));
  expr->kind = kind;
  expr->loc = loc;
  return expr;
}

uint64_t evalIntegerConstExpr(Expr *expr) {
  switch (expr->kind) {
  case EXPR_NUM:
    if (expr->num->kind != NUM_INT)
      break;
    return expr->num->i;

  case EXPR_ADD:
    return evalIntegerConstExpr(expr->x) + evalIntegerConstExpr(expr->y);

  case EXPR_SUB:
    return evalIntegerConstExpr(expr->x) - evalIntegerConstExpr(expr->y);

  default:
    break;
  }
  emitDiagnostic(expr->loc, "Expect an integer constant");
  assert(false && "Unreachable");
}

void printExpr(Expr *expr, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Expr [%d:%d] ", expr->loc->lineno, expr->loc->col);

  bool printType = false;
  bool printX = false, printY = false, printZ = false;

  switch (expr->kind) {
  case EXPR_IDENT:
    printf("Ident '%s'\n", expr->ident);
    printType = true;
    break;

  case EXPR_NUM:
    printNumber(expr->num);
    printf("\n");
    printType = true;
    break;

  case EXPR_MEMBER:
    printf("Member '%s'\n", expr->ident);
    printType = printX = true;
    break;

  case EXPR_POSTFIX_INCR:
    printf("PostfixIncr\n");
    printType = printX = true;
    break;

  case EXPR_POSTFIX_DECR:
    printf("PostfixDecr\n");
    printType = printX = true;
    break;

  case EXPR_CALL:
    printf("Call\n");
    printCType(expr->ty, indent + 1);
    printExpr(expr->x, indent + 1);
    for (int i = 0; i < arrlen(expr->callArgs); i++)
      printExpr(expr->callArgs[i], indent + 1);
    break;

  case EXPR_PREFIX_INCR:
    printf("PrefixIncr\n");
    printType = printX = true;
    break;

  case EXPR_PREFIX_DECR:
    printf("PrefixDecr\n");
    printType = printX = true;
    break;

  case EXPR_POS:
    printf("Pos\n");
    printType = printX = true;
    break;

  case EXPR_NEG:
    printf("Neg\n");
    printType = printX = true;
    break;

  case EXPR_SIZEOF_TYPE:
    printf("SizeofType\n");
    printCType(expr->ty, indent + 1);
    for (int i = 0; i < indent + 1; i++)
      printf("  ");
    printf("Inner\n");
    printCType(expr->sizeofTy, indent + 2);
    break;

  case EXPR_SIZEOF_VAL:
    printf("SizeofVal\n");
    printType = printX = true;
    break;

  case EXPR_CAST:
    printf("Cast\n");
    printType = printX = true;
    break;

  case EXPR_MUL:
    printf("Mul\n");
    printType = printX = printY = true;
    break;

  case EXPR_DIV:
    printf("Div\n");
    printType = printX = printY = true;
    break;

  case EXPR_MOD:
    printf("Mod\n");
    printType = printX = printY = true;
    break;

  case EXPR_ADD:
    printf("Add\n");
    printType = printX = printY = true;
    break;

  case EXPR_SUB:
    printf("Sub\n");
    printType = printX = printY = true;
    break;

  case EXPR_COND:
    printf("Cond\n");
    printType = printX = printY = printZ = true;
    break;

  case EXPR_EQ_ASSIGN:
    printf("EqAssign\n");
    printType = printX = printY = true;
    break;

  case EXPR_ADD_EQ:
    printf("AddEq\n");
    printType = printX = printY = true;
    break;

  case EXPR_SUB_EQ:
    printf("SubEq\n");
    printType = printX = printY = true;
    break;

  case EXPR_COMMA:
    printf("Comma\n");
    printType = printX = printY = true;
    break;

  default:
    assert(false);
  }

  if (printType)
    printCType(expr->ty, indent + 1);
  if (printX)
    printExpr(expr->x, indent + 1);
  if (printY)
    printExpr(expr->y, indent + 1);
  if (printZ)
    printExpr(expr->z, indent + 1);
}
