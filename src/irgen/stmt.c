#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"
#include "irgen/decl.h"
#include "irgen/expr.h"
#include "irgen/irgen.h"
#include "parse/stmt.h"

static void genCmpdStmt(IRGenCtx *ctx, Stmt *stmt);

void genStmt(IRGenCtx *ctx, Stmt *stmt) {
  if (ctx->unreachable) {
    printf("unreachable statement\n");
    return;
  }

  switch (stmt->kind) {
  case STMT_EMPTY:
    return;

  case STMT_DECL:
    return genDeclaration(ctx, stmt->decl);

  case STMT_CMPD:;
    return genCmpdStmt(ctx, stmt);

  case STMT_EXPR:
    genExpr(ctx, stmt->expr);
    return;

  case STMT_RETURN:;
    IRInst *ret = newIRInst(IR_RET);
    ret->src1 = stmt->expr == NULL ? newValueVoid() : genExpr(ctx, stmt->expr);
    arrput(ctx->block->insts, ret);

    ctx->unreachable = true;
    arrput(ctx->func->exits, ctx->block);
    return;

  default:
    assert(false);
  }
}

static void genCmpdStmt(IRGenCtx *ctx, Stmt *stmt) {
  assert(stmt->kind == STMT_CMPD);

  SymTable *savedSymtab = ctx->symtab;
  ctx->symtab = stmt->symtab;

  for (int i = 0; i < arrlen(stmt->children); i++)
    genStmt(ctx, stmt->children[i]);

  ctx->symtab = savedSymtab;
}
