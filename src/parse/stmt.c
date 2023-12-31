#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "parse/symbol.h"
#include "parse/type.h"

static int parseLabel(ParseCtx *ctx, const Token *begin, Stmt *stmt);
static int parseReturnStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);

int parseStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  if (tokenIsPunct(p, PUNCT_SEMICOLON)) {
    stmt->kind = STMT_EMPTY;
    return 1;
  }

  if (begin->kind == TOK_IDENT && tokenIsPunct(begin + 1, PUNCT_COLON))
    return parseLabel(ctx, p, stmt);

  if (tokenIsPunct(p, PUNCT_BRACE_L))
    return parseCmpdStmt(ctx, p, stmt);

  if (tokenIsKwd(p, KWD_RETURN))
    return parseReturnStmt(ctx, p, stmt);

  stmt->decl = calloc(1, sizeof(Declaration));
  if ((n = parseDeclaration(ctx, p, stmt->decl)) == 0) {
    free(stmt->decl);
    stmt->decl = NULL;
  } else {
    p += n;
    stmt->kind = STMT_DECL;
    return p - begin;
  }

  if ((n = parseExpr(ctx, begin, EXPR_PREC_ALL, &stmt->expr)) != 0) {
    p += n;
    stmt->kind = STMT_EXPR;

    if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
      printf("expect semicolon\n");
      exit(1);
    }
    return p + 1 - begin;
  }

  return 0;
}

int parseCmpdStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  if (!tokenIsPunct(p, PUNCT_BRACE_L)) {
    printf("expect left brace\n");
    exit(1);
  }
  p++;
  stmt->kind = STMT_CMPD;

  SymTable *savedSymtab = ctx->symtab;
  if (stmt->symtab == NULL) {
    ctx->symtab = stmt->symtab = newSymTable(ctx->symtab);
  } else {
    assert(ctx->symtab == stmt->symtab);
  }

parse_compound_statement_begin:
  if (tokenIsPunct(p, PUNCT_BRACE_R)) {
    p++;
    ctx->symtab = savedSymtab;
    return p - begin;
  }

  Stmt *childStmt = calloc(1, sizeof(Stmt));
  if ((n = parseStmt(ctx, p, childStmt)) == 0) {
    free(childStmt);
    printf("expect declaration or statement\n");
    exit(1);
  }
  p += n;
  arrput(stmt->children, childStmt);

  goto parse_compound_statement_begin;
}

static int parseLabel(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  assert(begin->kind == TOK_IDENT && tokenIsPunct(begin + 1, PUNCT_COLON));
  stmt->kind = STMT_LABEL;
  stmt->label = begin->ident;

  if (symTableGetShallow(ctx->func->labelTable, stmt->label) != NULL) {
    printf("label %s already exist\n", stmt->label);
    exit(1);
  }
  symTablePut(ctx->func->labelTable, newSymbol(stmt->label, NULL));

  return 2;
}

static int parseReturnStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  assert(tokenIsKwd(p, KWD_RETURN));
  p++;
  stmt->kind = STMT_RETURN;

  if (ctx->func->decltors[0]->ty->func.ret->kind != TYPE_VOID) {
    if ((n = parseExpr(ctx, p, EXPR_PREC_ALL, &stmt->expr)) == 0) {
      printf("expect expression\n");
      exit(1);
    }
    p += n;
  }

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    printf("expect semicolon\n");
    exit(1);
  }
  return p + 1 - begin;
}

void printStmt(Stmt *stmt, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");

  switch (stmt->kind) {
  case STMT_EMPTY:
    printf("Stmt Empty\n");
    return;

  case STMT_LABEL:
    printf("Stmt Label '%s'\n", stmt->label);
    return;

  case STMT_DECL:
    printf("Stmt Declaration\n");
    printDeclaration(stmt->decl, indent + 1);
    return;

  case STMT_CMPD:
    printf("Stmt Compound\n");
    for (int i = 0; i < arrlen(stmt->children); i++)
      printStmt(stmt->children[i], indent + 1);
    return;

  case STMT_EXPR:
    printf("Stmt Expr\n");
    printExpr(stmt->expr, indent + 1);
    return;

  case STMT_RETURN:
    printf("Stmt Return\n");
    if (stmt->expr != NULL)
      printExpr(stmt->expr, indent + 1);
    return;
  }
}
