#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "sema/decl.h"
#include "sema/type.h"
#include "util/align.h"

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

  // RISC-V Calling Conventions 4.1. C/C++ type sizes and alignments
  case TYPE_BOOL:
  case TYPE_CHAR:
    ty->size = 1;
    ty->align = 1;
    return;

  case TYPE_SHORT:
    ty->size = 2;
    ty->align = 2;
    return;

  case TYPE_INT:
  case TYPE_FLOAT:
    ty->size = 4;
    ty->align = 4;
    return;

  case TYPE_LONG:
  case TYPE_LONG_LONG:
  case TYPE_DOUBLE:
  case TYPE_PTR:
    ty->size = 8;
    ty->align = 8;
    return;

  case TYPE_LONG_DOUBLE:
    ty->size = 16;
    ty->align = 16;
    return;

  case TYPE_COMPLEX:
    switch (ty->complex) {
    case TYPE_FLOAT:
      ty->align = 4;
      break;
    case TYPE_DOUBLE:
      ty->align = 8;
      break;
    case TYPE_LONG_DOUBLE:
      ty->align = 16;
      break;
    default:
      assert(false && "Unknown complex type");
    }
    ty->size = ty->align * 2;
    return;

  case TYPE_STRUCT:
    ty->size = 0;
    ty->align = 1;
    for (int i = 0; i < arrlen(ty->struc.decltions); i++) {
      Declaration *decltion = ty->struc.decltions[i];
      for (int j = 0; j < arrlen(decltion->decltors); j++) {
        Declarator *decltor = decltion->decltors[j];
        decltor->offset = alignTo(ty->size, decltor->ty->align);
        if (decltor->ty->align > ty->align) {
          assert(decltor->ty->align % ty->align == 0);
          ty->align = decltor->ty->align;
        }
        ty->size = decltor->offset + decltor->ty->size;
      }
    }
    return;

  default:
    assert(false);
  }
}

// C99 6.2.5 Types (4), (6) and (7)
bool typeIsInteger(CType *ty) {
  switch (ty->kind) {
  case TYPE_BOOL:
  case TYPE_CHAR:
  case TYPE_SHORT:
  case TYPE_INT:
  case TYPE_LONG:
  case TYPE_LONG_LONG:
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
  return typeIsReal(ty) || ty->kind == TYPE_COMPLEX;
}

bool typeIsReal(CType *ty) {
  if (typeIsInteger(ty))
    return true;
  switch (ty->kind) {
  case TYPE_FLOAT:
  case TYPE_DOUBLE:
  case TYPE_LONG_DOUBLE:
    return true;
  default:
    return false;
  }
}

// C99 6.2.5 Types (21): Arithmetic types and pointer types are collectively
// called scalar types.
bool typeIsScarlar(CType *ty) {
  return ty->kind == TYPE_PTR || typeIsArithmetic(ty);
}

bool typeIsModifiableLvalue(CType *ty) {
  // TODO: Check lvalues that are not modifiable.
  switch (ty->kind) {
  case TYPE_FUNC:
    return false;
  default:
    return ty->attr & TYPE_ATTR_LVALUE;
  }
}

bool integerTypeSame(CType *ty1, CType *ty2) {
  assert(typeIsInteger(ty1) && typeIsInteger(ty2));
  int ty1U = ty1->attr & TYPE_ATTR_UNSIGNED;
  int ty2U = ty2->attr & TYPE_ATTR_UNSIGNED;
  return ty1->kind == ty2->kind && ty1U == ty2U;
}

// C99 6.3.1.1 Boolean, characters, and integers (1), or:
// https://en.cppreference.com/w/c/language/conversion#Integer_promotions
static int integerRank(CTypeKind kind) {
  switch (kind) {
  case TYPE_BOOL:
    return 5;
  case TYPE_CHAR:
    return 10;
  case TYPE_INT:
    return 20;
  case TYPE_SHORT:
    return 30;
  case TYPE_LONG:
    return 40;
  case TYPE_LONG_LONG:
    return 50;
  default:
    assert(false);
  }
}

// C99 6.3.1.1 Boolean, characters, and integers (2), or:
// https://en.cppreference.com/w/c/language/conversion#Integer_promotions
CType *integerPromote(CType *ty) {
  // TODO: Add support fot bit fields.
  assert(typeIsInteger(ty));

  // ... All other types are unchanged by the integer promotions.
  if (integerRank(ty->kind) >= integerRank(TYPE_INT)) {
    // The promoted type should not be an lvalue.
    return newCType(ty->kind, ty->attr & (~TYPE_ATTR_LVALUE));
  }

  // If an int can represent all values of the original type, the value is
  // converted to an int; otherwise, it is converted to an unsigned int.
  //
  // The value is always converted to an int, I think?
  return newCType(TYPE_INT, TYPE_ATTR_NONE);
}

// C99 6.3.1.8 Usual arithmetic conversions
CType *commonRealCType(CType *ty1, CType *ty2) {
  // TODO: Add support for real floating types.
  assert(typeIsInteger(ty1) && typeIsInteger(ty2));

  // The integer promotions are performed on both operands.
  CType *pty1 = integerPromote(ty1);
  CType *pty2 = integerPromote(ty2);

  int ty1U = pty1->attr & TYPE_ATTR_UNSIGNED;
  int ty2U = pty2->attr & TYPE_ATTR_UNSIGNED;
  // If both operands have the same type, then no further conversion is needed.
  if (pty1->kind == pty2->kind && ty1U == ty2U)
    return newCType(pty1->kind, ty1U);

  // Otherwise, if both operands have signed integer types or both have unsigned
  // integer types, the operand with the type of lesser integer conversion rank
  // is converted to the type of the operand with greater rank.
  if (ty1U == ty2U) {
    int rank1 = integerRank(pty1->kind);
    int rank2 = integerRank(pty2->kind);
    if (rank1 > rank2)
      return newCType(pty1->kind, ty1U);
    return newCType(pty2->kind, ty2U);
  }

  // Otherwise, if the operand that has unsigned integer type has rank greater
  // or equal to the rank of the type of the other operand, then the operand
  // with signed integer type is converted to the type of the operand with
  // unsigned integer type.
  CType *tyU = pty1->attr & TYPE_ATTR_UNSIGNED ? pty1 : pty2;
  CType *tyS = pty1->attr & TYPE_ATTR_UNSIGNED ? pty2 : pty1;
  int rankU = integerRank(tyU->kind);
  int rankS = integerRank(tyS->kind);
  if (rankU >= rankS)
    return newCType(tyU->kind, TYPE_ATTR_UNSIGNED);

  // Otherwise, if the type of the operand with signed integer type can
  // represent all of the values of the type of the operand with unsigned
  // integer type, then the operand with unsigned integer type is converted to
  // the type of the operand with signed integer type.
  //
  // This is always true, I think?
  return newCType(tyS->kind, TYPE_ATTR_NONE);

  // Otherwise, both operands are converted to the unsigned integer type
  // corresponding to the type of the operand with signed integer type.
}

void printCType(CType *ty, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Type size=%d align=%d ", ty->size, ty->align);

  if (ty->attr & TYPE_ATTR_LVALUE)
    printf("lvalue ");
  if (ty->attr & TYPE_ATTR_UNSIGNED)
    printf("unsigned ");

  switch (ty->kind) {
  case TYPE_UNTYPED:
    printf("<untyped>\n");
    return;

  case TYPE_BOOL:
    printf("_Bool\n");
    return;

  case TYPE_CHAR:
    if (!(ty->attr & TYPE_ATTR_UNSIGNED))
      printf("signed ");
    printf("char\n");
    return;

  case TYPE_SHORT:
    printf("short\n");
    return;

  case TYPE_INT:
    printf("int\n");
    return;

  case TYPE_LONG:
    printf("long\n");
    return;

  case TYPE_LONG_LONG:
    printf("long long\n");
    return;

  case TYPE_VOID:
    printf("void\n");
    return;

  case TYPE_FLOAT:
    printf("float\n");
    return;

  case TYPE_DOUBLE:
    printf("double\n");
    return;

  case TYPE_LONG_DOUBLE:
    printf("long double\n");
    return;

  case TYPE_COMPLEX:
    switch (ty->complex) {
    case TYPE_FLOAT:
      printf("float _Complex\n");
      return;
    case TYPE_DOUBLE:
      printf("double _Complex\n");
      return;
    case TYPE_LONG_DOUBLE:
      printf("long double _Complex\n");
      return;
    default:
      assert(false && "Unknown complex type");
    }

  case TYPE_STRUCT:
    printf("struct\n");
    for (int i = 0; i < arrlen(ty->struc.decltions); i++)
      printDeclaration(ty->struc.decltions[i], indent + 1, true);
    return;

  case TYPE_FUNC:
    printf("function\n");
    printCType(ty->func.ret, indent + 1);
    for (int i = 0; i < arrlen(ty->func.params); i++)
      printDeclarator(ty->func.params[i], indent + 1, false);
    return;

  case TYPE_PTR:
    printf("pointer\n");
    printCType(ty->ptr.inner, indent + 1);
    return;

  default:
    assert(false);
  }
}
