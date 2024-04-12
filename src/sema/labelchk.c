#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "lex/token.h"
#include "sema/ctx.h"
#include "sema/decl.h"
#include "sema/stmt.h"
#include "sema/symbol.h"
#include "sema/transunit.h"
#include "util/diag.h"

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

  if (!symTableGet(ctx->func->labelTable, stmt->label)) {
    emitDiagnostic(stmt->labelLoc, "Undefined label %s", stmt->label);
  }
}
