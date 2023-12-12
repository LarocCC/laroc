#include <assert.h>
#include <string.h>

#include "stb_ds.h"

#include "lex/lex.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/stmt.h"

int main() {
  const char *source;
  Token *tokens;
  Stmt stmt;

  source = "{}";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseCmpdStmt(tokens, &stmt) == 2);
  assert(stmt.kind == STMT_CMPD);

  source = "{ int a; }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseCmpdStmt(tokens, &stmt) == 5);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 1);
  assert(stmt.children[0]->kind == STMT_DECL);

  source = "{ int a; { int b; } }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseCmpdStmt(tokens, &stmt) == 10);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 2);
  assert(stmt.children[0]->kind == STMT_DECL);
  assert(stmt.children[1]->kind == STMT_CMPD);

  source = "{ {} {} {} }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseCmpdStmt(tokens, &stmt) == 8);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 3);
  assert(stmt.children[0]->kind == STMT_CMPD);
  assert(stmt.children[1]->kind == STMT_CMPD);
  assert(stmt.children[2]->kind == STMT_CMPD);

  return 0;
}
