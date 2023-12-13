#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "stb_ds.h"

#include "parse/symbol.h"
#include "typedef.h"
#include "lex/lex.h"
#include "lex/number.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/stmt.h"
#include "parse/parse.h"
#include "parse/type.h"

int main() {
  const char *source;
  Token *tokens;

  ParseCtx ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.symtab = newSymTable(NULL);

  Declarator decltor;

  source = ";";
  tokens = lex(source, strlen(source));
  assert(parseDeclarator(&ctx, tokens, &decltor) == 0);

  source = "laroc";
  tokens = lex(source, strlen(source));
  assert(parseDeclarator(&ctx, tokens, &decltor) == 1);
  assert(decltor.ty->kind == TYPE_UNTYPED);
  assert(strcmp(decltor.ident, "laroc") == 0);

  source = "laroc()";
  tokens = lex(source, strlen(source));
  assert(parseDeclarator(&ctx, tokens, &decltor) == 3);
  assert(decltor.ty->kind == TYPE_FUNC);
  assert(strcmp(decltor.ident, "laroc") == 0);

  source = "add(int x, int y)";
  tokens = lex(source, strlen(source));
  assert(parseDeclarator(&ctx, tokens, &decltor) == 8);
  assert(decltor.ty->kind == TYPE_FUNC);
  assert(arrlen(decltor.ty->func.params) == 2);

  Declaration decltion;

  source = "int laroc;";
  tokens = lex(source, strlen(source));
  memset(&decltion, 0, sizeof(Declaration));
  assert(parseDeclaration(&ctx, tokens, &decltion) == 3);
  assert(arrlen(decltion.decltors) == 1);
  assert(strcmp(decltion.decltors[0]->ident, "laroc") == 0);
  assert(decltion.decltors[0]->ty->kind == TYPE_INT);

  source = "int ans = 42, code = 200;";
  tokens = lex(source, strlen(source));
  memset(&decltion, 0, sizeof(Declaration));
  assert(parseDeclaration(&ctx, tokens, &decltion) == 9);
  assert(arrlen(decltion.decltors) == 2);
  assert(strcmp(decltion.decltors[0]->ident, "ans") == 0);
  assert(decltion.decltors[0]->ty->kind == TYPE_INT);
  assert(decltion.decltors[0]->init->kind == EXPR_NUM);
  assert(decltion.decltors[0]->init->num->x == 42);
  assert(strcmp(decltion.decltors[1]->ident, "code") == 0);
  assert(decltion.decltors[1]->ty->kind == TYPE_INT);
  assert(decltion.decltors[1]->init->kind == EXPR_NUM);
  assert(decltion.decltors[1]->init->num->x == 200);

  source = "int a, b, c();";
  tokens = lex(source, strlen(source));
  memset(&decltion, 0, sizeof(Declaration));
  assert(parseDeclaration(&ctx, tokens, &decltion) == 9);
  assert(arrlen(decltion.decltors) == 3);
  assert(strcmp(decltion.decltors[0]->ident, "a") == 0);
  assert(decltion.decltors[0]->ty->kind == TYPE_INT);
  assert(strcmp(decltion.decltors[1]->ident, "b") == 0);
  assert(decltion.decltors[1]->ty->kind == TYPE_INT);
  assert(strcmp(decltion.decltors[2]->ident, "c") == 0);
  assert(decltion.decltors[2]->ty->kind == TYPE_FUNC);
  assert(decltion.decltors[2]->ty->func.ret->kind == TYPE_INT);

  source = "int main() {}";
  tokens = lex(source, strlen(source));
  memset(&decltion, 0, sizeof(Declaration));
  assert(parseDeclaration(&ctx, tokens, &decltion) == 6);
  assert(decltion.funcDef->kind == STMT_CMPD);
  assert(arrlen(decltion.decltors) == 1);
  assert(strcmp(decltion.decltors[0]->ident, "main") == 0);
  assert(decltion.decltors[0]->ty->kind == TYPE_FUNC);
  assert(decltion.decltors[0]->ty->func.ret->kind == TYPE_INT);

  return 0;
}
