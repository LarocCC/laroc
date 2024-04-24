#ifndef LAROC_SEMA_TYPE_H
#define LAROC_SEMA_TYPE_H

#include <stdbool.h>

#include "typedef.h"

// CTypeKind describes the C99 6.2.4 Types (25) type category of a type.
typedef enum CTypeKind {
  TYPE_UNTYPED,

  // C99 6.2.5 Types (6): The type _Bool and (...) are the standard unsigned
  // integer types.
  //
  // A CType with TYPE_BOOL kind always has TYPE_ATTR_UNSIGNED attr set.
  TYPE_BOOL,

  // RISC-V Calling Conventions 4.2 C/C++ type representations: `char` is
  // unsigned.
  //
  // We use a CType with TYPE_CHAR kind and *without*
  // TYPE_ATTR_UNSIGNED attr to represent a `signed char`; use a CType with
  // TYPE_CHAR kind *and* TYPE_ATTR_UNSIGNED attr to represent a `char` or a
  // `unsigned char`.
  TYPE_CHAR,
  TYPE_SHORT,
  TYPE_INT,
  TYPE_LONG,
  TYPE_LONG_LONG,

  TYPE_VOID,

  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_LONG_DOUBLE,

  TYPE_COMPLEX,

  // TODO: Add C99 6.2.5 Types (16) enumerated type.

  // Derived types
  TYPE_ARRAY,
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_FUNC,
  TYPE_PTR,
} CTypeKind;

typedef enum CTypeAttr {
  TYPE_ATTR_NONE = 0,

  /// This integer type is unsigned.
  TYPE_ATTR_UNSIGNED = 1 << 0,

  /// This type is an lvalue.
  TYPE_ATTR_LVALUE = 1 << 1,
} CTypeAttr;

/// A C type.
struct CType {
  CTypeKind kind;
  CTypeAttr attr;
  int size, align;

  CTypeKind complex;

  struct {
    const char *ident;
    SymTable *symtab;
    Declaration **decltions;
  } struc;

  struct {
    CType *ret;
    Declarator **params;
  } func;

  struct {
    CType *inner;
  } ptr;
};

CType *newCType(CTypeKind kind, CTypeAttr attr);

/// Compute and set ty->size and ty->align.
void computeCTypeSize(CType *ty);

bool typeIsInteger(CType *ty);
bool typeIsArithmetic(CType *ty);
bool typeIsReal(CType *ty);
bool typeIsScarlar(CType *ty);

bool typeIsModifiableLvalue(CType *ty);

bool integerTypeSame(CType *ty1, CType *ty2);

CType *integerPromote(CType *ty);

CType *commonRealCType(CType *ty1, CType *ty2);

void printCType(CType *ty, int indent);

#endif
