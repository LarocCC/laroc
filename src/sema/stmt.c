#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "sema/decl.h"
#include "sema/expr.h"
#include "sema/stmt.h"

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
    printDeclaration(stmt->decl, indent + 1, false);
    return;

  case STMT_CMPD:
    printf("Stmt Compound\n");
    for (int i = 0; i < arrlen(stmt->children); i++)
      printStmt(stmt->children[i], indent + 1);
    return;

  case STMT_EXPR:
    printf("Stmt Expr\n");
    printExpr(stmt->expr1, indent + 1);
    return;

  case STMT_IF:
    printf("Stmt If\n");
    printExpr(stmt->expr1, indent + 1);
    printStmt(stmt->stmt1, indent + 1);
    if (stmt->stmt2)
      printStmt(stmt->stmt2, indent + 1);
    return;

  case STMT_GOTO:
    printf("Stmt Goto '%s'\n", stmt->label);
    return;

  case STMT_RETURN:
    printf("Stmt Return\n");
    if (stmt->expr1)
      printExpr(stmt->expr1, indent + 1);
    return;

  default:
    assert(false);
  }
}
