#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "parse/symbol.h"

int parseStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt) {
  const Token *p = begin;
  int n;

  if (tokenIsPunct(p, PUNCT_BRACE_L))
    return parseCmpdStmt(ctx, p, stmt);

  if (tokenIsPunct(p, PUNCT_SEMICOLON)) {
    stmt->kind = STMT_EMPTY;
    return 1;
  }

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
  ctx->symtab = stmt->symtab = newSymTable(ctx->symtab);

parse_compound_statement_begin:
  if (tokenIsPunct(p, PUNCT_BRACE_R)) {
    p++;
    ctx->symtab = ctx->symtab->parent;
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
