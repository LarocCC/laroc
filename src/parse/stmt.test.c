#include <assert.h>
#include <string.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/lex.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "parse/symbol.h"

int main() {
  const char *source;
  Token *tokens;
  Stmt stmt;

  ParseCtx ctx;
  memset(&ctx, 0, sizeof(ParseCtx));
  ctx.symtab = newSymTable(NULL);

  source = "{}";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 2);
  assert(stmt.kind == STMT_CMPD);

  source = ";";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 1);
  assert(stmt.kind == STMT_EMPTY);

  source = "42;";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 2);
  assert(stmt.kind == STMT_EXPR);
  assert(stmt.expr->kind == EXPR_NUM);

  source = "{ int a; }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 5);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 1);
  assert(stmt.children[0]->kind == STMT_DECL);

  source = "{ int a; { int b; } }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 10);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 2);
  assert(stmt.children[0]->kind == STMT_DECL);
  assert(stmt.children[1]->kind == STMT_CMPD);

  source = "{ {} {} {} }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 8);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 3);
  assert(stmt.children[0]->kind == STMT_CMPD);
  assert(stmt.children[1]->kind == STMT_CMPD);
  assert(stmt.children[2]->kind == STMT_CMPD);

  source = "{ answer = 6 * 7; }";
  tokens = lex(source, strlen(source));
  memset(&stmt, 0, sizeof(Stmt));
  assert(parseStmt(&ctx, tokens, &stmt) == 8);
  assert(stmt.kind == STMT_CMPD);
  assert(arrlen(stmt.children) == 1);
  assert(stmt.children[0]->kind == STMT_EXPR);

  return 0;
}
