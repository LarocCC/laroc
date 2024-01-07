#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/token.h"
#include "sema/type.h"

int parseSpecifier(const Token *begin, CType *ty) {
  typedef enum {
    SPEC_NONE = 0,
    SPEC_VOID = 1 << 0,
    SPEC_INT = 1 << 1,
  } Specifier;
  Specifier spec = SPEC_NONE;

  const Token *p = begin;

parse_specifier_begin:
  if (tokenIsKwd(p, KWD_VOID)) {
    if (spec & SPEC_VOID) {
      printf("invalid \"void \"");
      exit(1);
    }
    spec |= SPEC_VOID;
    p++;
    goto parse_specifier_begin;
  }

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
  case SPEC_VOID:
    ty->kind = TYPE_VOID;
    break;
  case SPEC_INT:
    ty->kind = TYPE_INT;
    break;
  default:
    printf("invalid type\n");
    exit(1);
  }

  computeCTypeSize(ty);
  return p - begin;
}

CType *fillUntyped(CType *root, CType *val) {
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
