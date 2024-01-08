#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "sema/ctx.h"
#include "sema/decl.h"
#include "sema/expr.h"
#include "sema/stmt.h"
#include "sema/transunit.h"

static void visitFunc(SemaCtx *ctx, Declaration *func);
static void visitDecl(SemaCtx *ctx, Declaration *decltion);
static void visitStmt(SemaCtx *ctx, Stmt *stmt);
static void visitExpr(SemaCtx *ctx, Expr *expr);

SemaCtx *newSemaCtx(TranslationUnit *unit) {
  SemaCtx *ctx = calloc(1, sizeof(SemaCtx));
  ctx->unit = unit;
  return ctx;
}

void visitTranslationUnit(SemaCtx *ctx) {
  ctx->symtab = ctx->unit->symtab;

  for (int i = 0; i < arrlen(ctx->unit->decltions); i++) {
    Declaration *decltion = ctx->unit->decltions[i];
    if (decltion->funcDef)
      visitFunc(ctx, decltion);
    else
      visitDecl(ctx, decltion);
  }

  ctx->symtab = NULL;
}

static void visitFunc(SemaCtx *ctx, Declaration *func) {
  SymTable *savedSymtab = ctx->symtab;
  ctx->func = func;
  ctx->symtab = func->funcDef->symtab;

  if (ctx->funcVisitor)
    ctx->funcVisitor(ctx, func);

  visitStmt(ctx, func->funcDef);

  ctx->func = NULL;
  ctx->symtab = savedSymtab;
}

static void visitDecl(SemaCtx *ctx, Declaration *decltion) {
  assert(!decltion->funcDef
         && "Use visitFunc() instead for function definations");

  for (int i = 0; i < arrlen(decltion->decltors); i++) {
    ctx->decl = decltion->decltors[i];

    if (ctx->decl->init)
      visitExpr(ctx, ctx->decl->init);
    if (ctx->declVisitor)
      ctx->declVisitor(ctx, ctx->decl);

    ctx->decl = NULL;
  }
}

static void visitStmt(SemaCtx *ctx, Stmt *stmt) {
  switch (stmt->kind) {
  case STMT_DECL:
    assert(!stmt->decl->funcDef);
    visitDecl(ctx, stmt->decl);
    break;

  case STMT_CMPD:;
    SymTable *savedSymtab = ctx->symtab;
    ctx->symtab = stmt->symtab;
    for (int i = 0; i < arrlen(stmt->children); i++)
      visitStmt(ctx, stmt->children[i]);
    ctx->symtab = savedSymtab;
    break;

  case STMT_EXPR:
    visitExpr(ctx, stmt->expr1);
    break;

  case STMT_IF:
    visitExpr(ctx, stmt->expr1);
    visitStmt(ctx, stmt->stmt1);
    if (stmt->stmt2)
      visitStmt(ctx, stmt->stmt2);
    break;

  case STMT_RETURN:
    if (stmt->expr1)
      visitExpr(ctx, stmt->expr1);
    break;

  default:
    break;
  }

  if (ctx->stmtVisitor)
    ctx->stmtVisitor(ctx, stmt);
}

static void visitExpr(SemaCtx *ctx, Expr *expr) {
  if (expr->x)
    visitExpr(ctx, expr->x);
  if (expr->y)
    visitExpr(ctx, expr->y);

  if (ctx->exprVisitor)
    ctx->exprVisitor(ctx, expr);
}
