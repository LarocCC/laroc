#ifndef LAROC_PARSE_STMT_H
#define LAROC_PARSE_STMT_H

#include "typedef.h"

enum StmtKind {
  STMT_EMPTY,  // ;
  STMT_DECL,   // decl;
  STMT_CMPD,   // { children... }, with symtab
  STMT_EXPR,   // expr;
  STMT_RETURN, // return [expr]_opt;
};

struct Stmt {
  StmtKind kind;

  Declaration *decl;

  SymTable *symtab;
  Stmt **children;

  Expr *expr;
};

int parseStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);
int parseCmpdStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);

#endif
