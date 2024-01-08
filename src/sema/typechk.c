#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "lex/number.h"
#include "sema/ctx.h"
#include "sema/expr.h"
#include "sema/stmt.h"
#include "sema/symbol.h"
#include "sema/transunit.h"
#include "sema/type.h"

static void setStmtType(SemaCtx *ctx, Stmt *stmt);
static void setExprCType(SemaCtx *ctx, Expr *expr);

void semaTypeCheck(TranslationUnit *unit) {
  SemaCtx *ctx = newSemaCtx(unit);
  ctx->stmtVisitor = setStmtType;
  ctx->exprVisitor = setExprCType;
  visitTranslationUnit(ctx);
  free(ctx);
}

static void setStmtType(SemaCtx *ctx, Stmt *stmt) {
  switch (stmt->kind) {
  case STMT_IF:
    if (!typeIsScarlar(stmt->expr1->ty)) {
      printf("The controlling expression of an if statement shall have scalar "
             "type\n");
      exit(1);
    }

  default:
    break;
  }
}

static void setExprCType(SemaCtx *ctx, Expr *expr) {
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
    setExprCType(ctx, expr->x);
    setExprCType(ctx, expr->y);
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return;
    }
    break;

  case EXPR_ADD:
    setExprCType(ctx, expr->x);
    setExprCType(ctx, expr->y);
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return;
    }
    break;

  case EXPR_SUB:
    setExprCType(ctx, expr->x);
    setExprCType(ctx, expr->y);
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      return;
    }
    break;

  case EXPR_EQ_ASSIGN:
    setExprCType(ctx, expr->x);
    setExprCType(ctx, expr->y);
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
    setExprCType(ctx, expr->x);
    setExprCType(ctx, expr->y);
    expr->ty = expr->y->ty;
    return;

  default:
    break;
  }

  printf("cannot determine type of the expression\n");
  exit(1);
}
