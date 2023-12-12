#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "lex/punct.h"
#include "typedef.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/type.h"

int parseDeclarator(const Token *begin, Declarator *decltor) {
  const Token *p = begin;

  if (p->kind == TOK_IDENT) {
    decltor->ty = newType(TYPE_UNTYPED);
    decltor->ident = p->ident;
    p++;
    return p - begin;
  }

  return 0;
}

int parseDeclaration(const Token *begin, Declaration *decltion) {
  const Token *p = begin;
  int n;

  Type *spec = calloc(1, sizeof(Type));
  n = parseSpecifier(p, spec);
  if (n == 0) {
    free(spec);
    return 0;
  }
  p += n;

parse_declaration_list_begin:;
  Declarator *decltor = calloc(1, sizeof(Declarator));
  n = parseDeclarator(p, decltor);
  if (n == 0) {
    free(decltor);
    return 0;
  }
  p += n;
  decltor->ty = fillUntyped(decltor->ty, spec);
  arrput(decltion->decltors, decltor);

  if (tokenIsPunct(p, PUNCT_COMMA)) {
    p++;
    goto parse_declaration_list_begin;
  }

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    printf("missing semicolon\n");
    exit(1);
  }
  p++;

  return p - begin;
}
