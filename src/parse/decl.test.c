#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/lex.h"
#include "parse/decl.h"
#include "parse/type.h"

int main() {
  const char *source;
  Token *tokens;

  Declarator decltor;

  source = ";";
  tokens = lex(source, strlen(source));
  assert(parseDeclarator(tokens, &decltor) == 0);

  source = "laroc";
  tokens = lex(source, strlen(source));
  assert(parseDeclarator(tokens, &decltor) == 1);
  assert(decltor.ty->kind == TYPE_UNTYPED);
  assert(strcmp(decltor.ident, "laroc") == 0);

  Declaration decltion;

  source = "int laroc;";
  tokens = lex(source, strlen(source));
  memset(&decltion, 0, sizeof(Declaration));
  assert(parseDeclaration(tokens, &decltion) == 3);
  assert(arrlen(decltion.decltors) == 1);
  assert(strcmp(decltion.decltors[0]->ident, "laroc") == 0);
  assert(decltion.decltors[0]->ty->kind == TYPE_INT);

  source = "int a, b, c;";
  tokens = lex(source, strlen(source));
  memset(&decltion, 0, sizeof(Declaration));
  assert(parseDeclaration(tokens, &decltion) == 7);
  assert(arrlen(decltion.decltors) == 3);
  assert(strcmp(decltion.decltors[0]->ident, "a") == 0);
  assert(decltion.decltors[0]->ty->kind == TYPE_INT);
  assert(strcmp(decltion.decltors[1]->ident, "b") == 0);
  assert(decltion.decltors[1]->ty->kind == TYPE_INT);
  assert(strcmp(decltion.decltors[2]->ident, "c") == 0);
  assert(decltion.decltors[2]->ty->kind == TYPE_INT);

  return 0;
}
