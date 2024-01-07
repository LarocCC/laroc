#include <stdio.h>
#include <stdlib.h>

#include "sema/symbol.h"
#include "sema/transunit.h"
#include "sema/stmt.h"
#include "sema/decl.h"
#include "sema/ctx.h"
#include "typedef.h"

static void checkStmt(SemaCtx *ctx, Stmt *stmt);

void semaLabelCheck(TranslationUnit *unit) {
  SemaCtx *ctx = newSemaCtx(unit);
  ctx->stmtVisitor = checkStmt;
  visitTranslationUnit(ctx);
  free(ctx);
  return;
}

static void checkStmt(SemaCtx *ctx, Stmt *stmt) {
  if (stmt->kind != STMT_GOTO)
    return;

  if (symTableGet(ctx->func->labelTable, stmt->label) == NULL) {
    printf("undefined label %s\n", stmt->label);
    exit(1);
  }
}
