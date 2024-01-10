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
  case TYPE_VOID:
  case TYPE_FUNC:
    ty->size = 0;
    ty->align = 0;
    return;

  // Reference:
  // https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/v1.0/riscv-cc.adoc#cc-type-sizes-and-alignments
  case TYPE_INT:
    ty->size = 4;
    ty->align = 4;
    return;

  default:
    assert(false);
  }
}

// C99 6.2.5 Types (4), (6) and (7)
bool typeIsInteger(CType *ty) {
  switch (ty->kind) {
  case TYPE_INT:
    return true;
  default:
    return false;
  }
}

// C99 6.2.5 Types (18): Integer and floating types are collectively called
// arithmetic types. Each arithmetic type belongs to one type domain: the real
// type domain comprises the real types, the complex type domain comprises the
// complex types.
bool typeIsArithmetic(CType *ty) {
  // TODO: Add other arithmetic types.
  switch (ty->kind) {
  case TYPE_INT:
    return true;
  default:
    return false;
  }
}

bool typeIsReal(CType *ty) {
  // TODO: Add other real types.
  switch (ty->kind) {
  case TYPE_INT:
    return true;
  default:
    return false;
  }
}

// C99 6.2.5 Types (21): Arithmetic types and pointer types are collectively
// called scalar types.
bool typeIsScarlar(CType *ty) {
  // TODO: Add pointer type.
  return typeIsArithmetic(ty);
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

// C99 6.3.1.8 Usual arithmetic conversions
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
