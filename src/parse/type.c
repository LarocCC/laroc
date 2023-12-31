#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/type.h"

CType *newCType(CTypeKind kind) {
  CType *ty = calloc(1, sizeof(CType));
  ty->kind = kind;
  computeCTypeSize(ty);
  return ty;
}

void computeCTypeSize(CType *ty) {
  switch (ty->kind) {
  case TYPE_UNTYPED:
    ty->size = 0;
    ty->align = 0;
    return;
  case TYPE_VOID:
    ty->size = 0;
    ty->align = 0;
    return;
  case TYPE_INT:
    ty->size = 4;
    ty->align = 4;
    return;
  case TYPE_FUNC:
    ty->size = 0;
    ty->align = 0;
    return;
  default:
    assert(false);
  }
}

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

bool typeIsInteger(CType *ty) {
  switch (ty->kind) {
  case TYPE_INT:
    return true;
  default:
    return false;
  }
}

bool typeIsReal(CType *ty) {
  switch (ty->kind) {
  case TYPE_INT:
    return true;
  default:
    return false;
  }
}

bool typeIsArithmetic(CType *ty) {
  switch (ty->kind) {
  case TYPE_INT:
    return true;
  default:
    return false;
  }
}

bool typeIsModifiableLvalue(CType *ty) {
  return ty->isLvalue;
}

bool typeSame(CType *ty1, CType *ty2) {
  if (ty1->kind != ty2->kind)
    return false;

  switch (ty1->kind) {
  case TYPE_INT:
    return true;

  case TYPE_FUNC:
    if (!typeSame(ty1->func.ret, ty2->func.ret))
      return false;
    if (arrlen(ty1->func.params) != arrlen(ty2->func.params))
      return false;
    for (int i = 0; i < arrlen(ty1->func.params); i++) {
      if (!typeSame(ty1->func.params[i]->ty, ty2->func.params[i]->ty))
        return false;
    }
    return true;

  default:
    return false;
  }
}

CType *commonRealCType(CType *ty1, CType *ty2) {
  if (typeSame(ty1, ty2))
    return ty1;

  return newCType(TYPE_UNTYPED);
}

void printCType(CType *ty, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Type size=%d align=%d ", ty->size, ty->align);

  switch (ty->kind) {
  case TYPE_UNTYPED:
    printf("<untyped>\n");
    return;

  case TYPE_VOID:
    printf("void\n");
    return;

  case TYPE_INT:
    printf("int\n");
    return;

  case TYPE_FUNC:
    printf("function\n");
    printCType(ty->func.ret, indent + 1);
    for (int i = 0; i < arrlen(ty->func.params); i++)
      printDeclarator(ty->func.params[i], indent + 1);
    return;

  default:
    assert(false);
  }
}
