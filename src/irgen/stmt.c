#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "ir/func.h"
#include "ir/inst.h"
#include "ir/value.h"
#include "irgen/decl.h"
#include "irgen/expr.h"
#include "irgen/irgen.h"
#include "sema/decl.h"
#include "sema/stmt.h"
#include "sema/symbol.h"

static void genLabel(IRGenCtx *ctx, Stmt *stmt);
static void genIfStmt(IRGenCtx *ctx, Stmt *stmt);
static void genCmpdStmt(IRGenCtx *ctx, Stmt *stmt);
static void genGotoStmt(IRGenCtx *ctx, Stmt *stmt);

void genStmt(IRGenCtx *ctx, Stmt *stmt) {
  if (stmt->kind == STMT_LABEL)
    return genLabel(ctx, stmt);

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
    genExpr(ctx, stmt->expr1);
    return;

  case STMT_IF:
    return genIfStmt(ctx, stmt);

  case STMT_GOTO:
    return genGotoStmt(ctx, stmt);

  case STMT_RETURN:;
    IRInst *ret = newIRInst(IR_RET);
    arrput(ret->srcs, stmt->expr1 ? genExpr(ctx, stmt->expr1) : newValueVoid());
    irBlockAddInst(ctx->block, ret);

    ctx->unreachable = true;
    arrput(ctx->irFunc->exits, ctx->block);
    return;

  default:
    assert(false);
  }
}

static void genLabel(IRGenCtx *ctx, Stmt *stmt) {
  assert(stmt->kind == STMT_LABEL);

  Symbol *label = symTableGet(ctx->cFunc->labelTable, stmt->label);
  if (!label->block)
    label->block = newIRBlock(ctx->irFunc);

  if (!ctx->unreachable) {
    arrput(label->block->preds, ctx->block);
    arrput(ctx->block->succs, label->block);

    IRInst *j = newIRInst(IR_J);
    arrput(j->srcs, newValueBlock(label->block));
    irBlockAddInst(ctx->block, j);
  }

  ctx->block = label->block;
  ctx->unreachable = false;
}

static void genCmpdStmt(IRGenCtx *ctx, Stmt *stmt) {
  assert(stmt->kind == STMT_CMPD);

  SymTable *savedSymtab = ctx->symtab;
  ctx->symtab = stmt->symtab;

  for (int i = 0; i < arrlen(stmt->children); i++)
    genStmt(ctx, stmt->children[i]);

  ctx->symtab = savedSymtab;
}

static void genIfStmt(IRGenCtx *ctx, Stmt *stmt) {
  assert(stmt->kind == STMT_IF);

  Value *cond = genExpr(ctx, stmt->expr1);
  IRBlock *blkT = newIRBlock(ctx->irFunc);
  IRBlock *blkF = newIRBlock(ctx->irFunc);
  IRBlock *blkTail = stmt->stmt2 ? newIRBlock(ctx->irFunc) : blkF;

  arrput(ctx->block->succs, blkT);
  arrput(ctx->block->succs, blkF);
  arrput(blkT->preds, ctx->block);
  arrput(blkF->preds, ctx->block);

  IRInst *br = newIRInst(IR_BR);
  arrput(br->srcs, cond);
  arrput(br->srcs, newValueBlock(blkT));
  arrput(br->srcs, newValueBlock(blkF));
  irBlockAddInst(ctx->block, br);

  ctx->block = blkT;
  ctx->unreachable = false;
  genStmt(ctx, stmt->stmt1);
  if (!ctx->unreachable) {
    arrput(blkT->succs, blkTail);
    arrput(blkTail->preds, blkT);

    IRInst *j = newIRInst(IR_J);
    arrput(j->srcs, newValueBlock(blkTail));
    irBlockAddInst(ctx->block, j);
  }
  ctx->block = NULL;

  if (stmt->stmt2) {
    ctx->block = blkF;
    ctx->unreachable = false;
    genStmt(ctx, stmt->stmt2);

    if (!ctx->unreachable) {
      arrput(blkF->succs, blkTail);
      arrput(blkTail->preds, blkF);

      IRInst *j = newIRInst(IR_J);
      arrput(j->srcs, newValueBlock(blkTail));
      irBlockAddInst(ctx->block, j);
    }
    ctx->block = NULL;
  }

  ctx->block = blkTail;
  ctx->unreachable = false;
}

static void genGotoStmt(IRGenCtx *ctx, Stmt *stmt) {
  assert(stmt->kind == STMT_GOTO);

  Symbol *label = symTableGet(ctx->cFunc->labelTable, stmt->label);
  if (!label->block)
    label->block = newIRBlock(ctx->irFunc);
  arrput(label->block->preds, ctx->block);
  arrput(ctx->block->succs, label->block);

  IRInst *j = newIRInst(IR_J);
  arrput(j->srcs, newValueBlock(label->block));
  irBlockAddInst(ctx->block, j);

  ctx->unreachable = true;
}
