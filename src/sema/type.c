#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "sema/decl.h"
#include "sema/type.h"

CType *newCType(CTypeKind kind, CTypeAttr attr) {
  CType *ty = calloc(1, sizeof(CType));
  ty->kind = kind;
  ty->attr = attr;
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

bool typeIsModifiableLvalue(CType *ty) { return ty->attr & TYPE_ATTR_LVALUE; }

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
    return newCType(ty1->kind, TYPE_ATTR_NONE);

  return newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
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
