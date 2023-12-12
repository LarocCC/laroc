#ifndef LAROC_PARSE_STMT_H
#define LAROC_PARSE_STMT_H

#include "typedef.h"

enum StmtKind {
  STMT_EMPTY,
  STMT_CMPD,
};

struct Stmt {
  StmtKind kind;
};

int parseCmpdStmt(const Token *begin, Stmt *stmt);

#endif
