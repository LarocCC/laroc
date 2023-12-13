#ifndef LAROC_PARSE_STMT_H
#define LAROC_PARSE_STMT_H

#include "typedef.h"

enum StmtKind {
  STMT_EMPTY,
  STMT_DECL,
  STMT_CMPD,
  STMT_EXPR,
};

struct Stmt {
  StmtKind kind;

  Declaration *decl;
  Stmt **children;
  Expr *expr;
};

int parseStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);
int parseCmpdStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);

#endif
