#ifndef LAROC_SEMA_STMT_H
#define LAROC_SEMA_STMT_H

#include "typedef.h"

typedef enum StmtKind {
  STMT_EMPTY,    // ;
  STMT_LABEL,    // label:
  STMT_DECL,     // decl;
  STMT_CMPD,     // { children... }, with symtab
  STMT_EXPR,     // expr1;

  // Selection statements
  STMT_IF,       // if (expr1) stmt1; [else stmt2]_opt;
  STMT_SWITCH,   // switch (expr1) stmt1;

  // Iteration statements
  STMT_WHILE,    // while (expr1) stmt1;
  STMT_DO,       // do stmt1; while (expr1);
  STMT_FOR,      // for (decl/expr1; expr2; expr3) stmt1;

  // Jump statements
  STMT_GOTO,     // goto labal;
  STMT_CONTINUE, // continue;
  STMT_BREAK,    // break;
  STMT_RETURN,   // return [expr1]_opt;
} StmtKind;

struct Stmt {
  StmtKind kind;

  const char *label;

  SymTable *symtab;
  Stmt **children;

  Declaration *decl;
  Expr *expr1, *expr2, *expr3;
  Stmt *stmt1, *stmt2;
};

void printStmt(Stmt *stmt, int indent);

#endif
