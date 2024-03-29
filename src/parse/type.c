#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/token.h"
#include "sema/type.h"

int parseSpecifier(const Token *begin, CType *ty) {
  typedef enum {
    SPEC_NONE = 0,
    SPEC_VOID = 1 << 0,
    SPEC_CHAR = 1 << 1,
    SPEC_SHORT = 1 << 2,
    SPEC_INT = 1 << 3,
    SPEC_LONG = 1 << 4,
    SPEC_LONG_LONG = 1 << 5,
    SPEC_FLOAT = 1 << 6,
    SPEC_DOUBLE = 1 << 7,
    SPEC_SIGNED = 1 << 8,
    SPEC_UNSIGNED = 1 << 9,
    SPEC_BOOL = 1 << 10,
    SPEC_COMPLEX = 1 << 11,
  } Specifier;
  Specifier spec = SPEC_NONE;

  const Token *p = begin;
  for (; p->kind == TOK_KWD; p++) {
    switch (p->kwd) {
    case KWD_VOID:
      if (spec & SPEC_VOID)
        goto reject_keyword;
      spec |= SPEC_VOID;
      continue;

    case KWD_CHAR:
      if (spec & SPEC_CHAR)
        goto reject_keyword;
      spec |= SPEC_CHAR;
      continue;

    case KWD_SHORT:
      if (spec & SPEC_SHORT)
        goto reject_keyword;
      spec |= SPEC_SHORT;
      continue;

    case KWD_INT:
      if (spec & SPEC_INT)
        goto reject_keyword;
      spec |= SPEC_INT;
      continue;

    case KWD_LONG:
      if (spec & SPEC_LONG_LONG)
        goto reject_keyword;
      if (spec & SPEC_LONG)
        spec = (spec & ~SPEC_LONG) | SPEC_LONG_LONG;
      else
        spec |= SPEC_LONG;
      continue;

    case KWD_FLOAT:
      if (spec & SPEC_FLOAT)
        goto reject_keyword;
      spec |= SPEC_FLOAT;
      continue;

    case KWD_DOUBLE:
      if (spec & SPEC_DOUBLE)
        goto reject_keyword;
      spec |= SPEC_DOUBLE;
      continue;

    case KWD_SIGNED:
      if (spec & SPEC_SIGNED)
        goto reject_keyword;
      spec |= SPEC_SIGNED;
      continue;

    case KWD_UNSIGNED:
      if (spec & SPEC_UNSIGNED)
        goto reject_keyword;
      spec |= SPEC_UNSIGNED;
      continue;

    case KWD_BOOL:
      if (spec & SPEC_BOOL)
        goto reject_keyword;
      spec |= SPEC_BOOL;
      continue;

    // TODO: KWD_FLOAT
    // TODO: KWD_DOUBLE
    // TODO: KWD_COMPLEX
    default:
      break;
    }

    assert(false && "unreachable");
  reject_keyword:
    printf("invalid \"%s\"\n", kwdInfo[p->kwd].str);
    exit(1);
  }

  if (p == begin)
    return 0;

// Suppress "Case value not in enumerated type 'Specifier'" from clang.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"

  switch (spec) {
  case SPEC_VOID:
    ty->kind = TYPE_VOID;
    break;

  case SPEC_CHAR:
  case SPEC_UNSIGNED | SPEC_CHAR:
    ty->attr |= TYPE_ATTR_UNSIGNED;
  case SPEC_SIGNED | SPEC_CHAR:
    ty->kind = TYPE_CHAR;
    break;

  case SPEC_UNSIGNED | SPEC_SHORT:
  case SPEC_UNSIGNED | SPEC_SHORT | SPEC_INT:
    ty->attr |= TYPE_ATTR_UNSIGNED;
  case SPEC_SHORT:
  case SPEC_SHORT | SPEC_INT:
  case SPEC_SIGNED | SPEC_SHORT:
  case SPEC_SIGNED | SPEC_SHORT | SPEC_INT:
    ty->kind = TYPE_SHORT;
    break;

  case SPEC_UNSIGNED:
  case SPEC_UNSIGNED | SPEC_INT:
    ty->attr |= TYPE_ATTR_UNSIGNED;
  case SPEC_INT:
  case SPEC_SIGNED:
  case SPEC_SIGNED | SPEC_INT:
    ty->kind = TYPE_INT;
    break;

  case SPEC_UNSIGNED | SPEC_LONG:
  case SPEC_UNSIGNED | SPEC_LONG | SPEC_INT:
    ty->attr |= TYPE_ATTR_UNSIGNED;
  case SPEC_LONG:
  case SPEC_LONG | SPEC_INT:
  case SPEC_SIGNED | SPEC_LONG:
  case SPEC_SIGNED | SPEC_LONG | SPEC_INT:
    ty->kind = TYPE_LONG;
    break;

  case SPEC_UNSIGNED | SPEC_LONG_LONG:
  case SPEC_UNSIGNED | SPEC_LONG_LONG | SPEC_INT:
    ty->attr |= TYPE_ATTR_UNSIGNED;
  case SPEC_LONG_LONG:
  case SPEC_LONG_LONG | SPEC_INT:
  case SPEC_SIGNED | SPEC_LONG_LONG:
  case SPEC_SIGNED | SPEC_LONG_LONG | SPEC_INT:
    ty->kind = TYPE_LONG_LONG;
    break;

  case SPEC_BOOL:
    ty->kind = TYPE_BOOL;
    ty->attr = TYPE_ATTR_UNSIGNED;
    break;

  default:
    printf("invalid type\n");
    exit(1);
  }

#pragma clang diagnostic pop

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
