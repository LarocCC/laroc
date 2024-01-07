#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "sema/ctx.h"
#include "sema/decl.h"
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
    if (decltion->funcDef != NULL)
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

  if (ctx->funcVisitor != NULL)
    ctx->funcVisitor(ctx, func);

  visitStmt(ctx, func->funcDef);

  ctx->func = NULL;
  ctx->symtab = savedSymtab;
}

static void visitDecl(SemaCtx *ctx, Declaration *decltion) {
  assert(decltion->funcDef == NULL
         && "Use visitFunc() instead for function definations");

  for (int i = 0; i < arrlen(decltion->decltors); i++) {
    ctx->decl = decltion->decltors[i];

    if (ctx->decl->init != NULL && ctx->exprVisitor != NULL)
      ctx->exprVisitor(ctx, ctx->decl->init);
    if (ctx->declVisitor != NULL)
      ctx->declVisitor(ctx, ctx->decl);

    ctx->decl = NULL;
  }
}

static void visitStmt(SemaCtx *ctx, Stmt *stmt) {
  if (ctx->stmtVisitor != NULL)
    ctx->stmtVisitor(ctx, stmt);

  switch (stmt->kind) {
  case STMT_DECL:
    assert(stmt->decl->funcDef == NULL);
    visitDecl(ctx, stmt->decl);
    break;

  case STMT_CMPD:;
    SymTable *savedSymtab = ctx->symtab;
    ctx->symtab = stmt->symtab;
    for (int i = 0; i < arrlen(stmt->children); i++)
      visitStmt(ctx, stmt);
    ctx->symtab = savedSymtab;
    break;

  case STMT_EXPR:
    visitExpr(ctx, stmt->expr);
    break;

  default:
    break;
  }
}

static void visitExpr(SemaCtx *ctx, Expr *expr) {
  if (ctx->exprVisitor != NULL)
    ctx->exprVisitor(ctx, expr);
}
