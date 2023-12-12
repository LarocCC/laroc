#include <stdio.h>
#include <stdlib.h>

#include "lex/punct.h"
#include "lex/token.h"
#include "parse/stmt.h"

int parseCmpdStmt(const Token *begin, Stmt *stmt) {
  const Token *p = begin;

  if (!tokenIsPunct(p, PUNCT_BRACE_L)) {
    printf("expect left brace\n");
    exit(1);
  }
  p++;
  stmt->kind = STMT_CMPD;

  if (!tokenIsPunct(p, PUNCT_BRACE_R)) {
    printf("expect right brace\n");
    exit(1);
  }
  p++;

  return p - begin;
}
