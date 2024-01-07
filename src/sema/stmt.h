#ifndef LAROC_SEMA_STMT_H
#define LAROC_SEMA_STMT_H

#include "typedef.h"

typedef enum StmtKind {
  STMT_EMPTY,  // ;
  STMT_LABEL,  // label:
  STMT_DECL,   // decl;
  STMT_CMPD,   // { children... }, with symtab
  STMT_EXPR,   // expr;
  STMT_GOTO,   // goto labal;
  STMT_RETURN, // return [expr]_opt;
} StmtKind;

struct Stmt {
  StmtKind kind;

  const char *label;

  Declaration *decl;

  SymTable *symtab;
  Stmt **children;

  Expr *expr;
};

void printStmt(Stmt *stmt, int indent);

#endif
