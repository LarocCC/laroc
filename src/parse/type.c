#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/parse.h"
#include "sema/decl.h"
#include "sema/symbol.h"
#include "sema/type.h"
#include "util/diag.h"

static int parseStructSpecifier(ParseCtx *ctx, const Token *begin, CType *ty);

int parseSpecifier(ParseCtx *ctx, const Token *begin, CType *ty) {
  if (tokenIsKwd(begin, KWD_STRUCT) || tokenIsKwd(begin, KWD_UNION)) {
    return parseStructSpecifier(ctx, begin, ty);
  }

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

    case KWD_COMPLEX:
      if (spec & SPEC_COMPLEX)
        goto reject_keyword;
      spec |= SPEC_COMPLEX;
      continue;

    default:
      break;
    }

    assert(false && "unreachable");
  reject_keyword:
    emitDiagnostic(p->loc, "Invalid keyword '%s'", kwdInfo[p->kwd].str);
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

  case SPEC_FLOAT:
    ty->kind = TYPE_FLOAT;
    break;

  case SPEC_DOUBLE:
    ty->kind = TYPE_DOUBLE;
    break;

  case SPEC_LONG | SPEC_DOUBLE:
    ty->kind = TYPE_LONG_DOUBLE;
    break;

  case SPEC_FLOAT | SPEC_COMPLEX:
    ty->kind = TYPE_COMPLEX;
    ty->complex = TYPE_FLOAT;
    break;

  case SPEC_DOUBLE | SPEC_COMPLEX:
    ty->kind = TYPE_COMPLEX;
    ty->complex = TYPE_DOUBLE;
    break;

  case SPEC_LONG | SPEC_DOUBLE | SPEC_COMPLEX:
    ty->kind = TYPE_COMPLEX;
    ty->complex = TYPE_LONG_DOUBLE;
    break;

  default:
    emitDiagnostic(begin->loc, "Invalid type");
  }

#pragma clang diagnostic pop

  computeCTypeSize(ty);
  return p - begin;
}

/// Parse a C99 (6.7.2.1) struct-or-union-specifier, starting from "struct" or
/// "union".
static int parseStructSpecifier(ParseCtx *ctx, const Token *begin, CType *ty) {
  const Token *p = begin;
  if (tokenIsKwd(begin, KWD_STRUCT)) {
    ty->kind = TYPE_STRUCT;
  } else if (tokenIsKwd(begin, KWD_UNION)) {
    ty->kind = TYPE_UNION;
  } else {
    assert(false);
  }
  p++;

  if (p->kind == TOK_IDENT) {
    ty->struc.ident = p->ident;
    p++;
  }

  if (!tokenIsPunct(p, PUNCT_BRACE_L)) {
    if (ty->struc.ident == NULL) {
      emitDiagnostic(p->loc, "Expect '{'");
    }
    return p - begin;
  }
  p++;

  SymTable *savedSymtab = ctx->symtab;
  ty->struc.symtab = newSymTable(NULL);
  ctx->symtab = ty->struc.symtab;

  while (!tokenIsPunct(p, PUNCT_BRACE_R)) {
    Declaration *decltion = calloc(1, sizeof(Declaration));
    int n = parseDeclaration(ctx, p, decltion);
    arrput(ty->struc.decltions, decltion);
    p += n;
  }
  p++;

  computeCTypeSize(ty);
  ctx->symtab = savedSymtab;
  return p - begin;
}

CType *fillUntyped(CType *root, CType *val) {
  if (root->kind == TYPE_UNTYPED) {
    if (val->kind != TYPE_FUNC) {
      val->attr |= root->attr & TYPE_ATTR_LVALUE;
    }
    free(root);
    return val;
  }
  if (root->kind == TYPE_PTR) {
    root->ptr.inner = fillUntyped(root->ptr.inner, val);
    return root;
  }
  if (root->kind == TYPE_FUNC) {
    root->func.ret = fillUntyped(root->func.ret, val);
    return root;
  }
  return root;
}
