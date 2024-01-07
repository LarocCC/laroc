#ifndef LAROC_SEMA_CTX_H
#define LAROC_SEMA_CTX_H

#include <stdbool.h>

#include "typedef.h"

typedef void CFuncVisitor(SemaCtx *ctx, Declaration *func);
typedef void DeclVisitor(SemaCtx *ctx, Declarator *decl);
typedef void StmtVisitor(SemaCtx *ctx, Stmt *stmt);
typedef void ExprVisitor(SemaCtx *ctx, Expr *expr);

struct SemaCtx {
  TranslationUnit *unit;
  Declaration *func;
  Declarator *decl;

  SymTable *symtab;

  CFuncVisitor *funcVisitor;
  DeclVisitor *declVisitor;
  StmtVisitor *stmtVisitor;
  ExprVisitor *exprVisitor;
};

SemaCtx *newSemaCtx(TranslationUnit *unit);

void visitTranslationUnit(SemaCtx *ctx);

#endif
