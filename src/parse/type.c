#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/token.h"
#include "parse/type.h"

Type *newType(TypeKind kind) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = kind;
  return ty;
}

int parseSpecifier(const Token *begin, Type *ty) {
  typedef enum {
    SPEC_NONE = 0,
    SPEC_INT = 1 << 0,
  } Specifier;
  Specifier spec = SPEC_NONE;

  const Token *p = begin;

parse_specifier_begin:
  if (tokenIsKwd(p, KWD_INT)) {
    if (spec & SPEC_INT) {
      printf("invalid \"int\"\n");
      exit(1);
    }
    spec |= SPEC_INT;
    p++;
    goto parse_specifier_begin;
  }

  if (p == begin)
    return 0;

  switch (spec) {
  case SPEC_INT:
    ty->kind = TYPE_INT;
    break;
  default:
    printf("invalid type\n");
    exit(1);
  }

  return p - begin;
}

Type *fillUntyped(Type *root, Type *val) {
  if (root->kind == TYPE_UNTYPED) {
    free(root);
    return val;
  }
  if (root->kind == TYPE_FUNC) {
    root->func.ret = fillUntyped(root->func.ret, val);
    return root;
  }
  return root;
}
