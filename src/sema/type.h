#ifndef LAROC_SEMA_TYPE_H
#define LAROC_SEMA_TYPE_H

#include <stdbool.h>

#include "typedef.h"

typedef enum CTypeKind {
  TYPE_UNTYPED,

  TYPE_VOID,

  TYPE_INT,

  TYPE_FUNC,
} CTypeKind;

typedef enum CTypeAttr {
  TYPE_ATTR_NONE = 0,
  TYPE_ATTR_LVALUE = 1 << 0,
} CTypeAttr;

struct CType {
  CTypeKind kind;
  CTypeAttr attr;
  int size, align;

  struct {
    CType *ret;
    Declarator **params;
  } func;
};

CType *newCType(CTypeKind kind, CTypeAttr attr);

/// Compute and set ty->size and ty->align.
void computeCTypeSize(CType *ty);

bool typeIsInteger(CType *ty);
bool typeIsReal(CType *ty);
bool typeIsArithmetic(CType *ty);
bool typeIsScarlar(CType *ty);

bool typeIsModifiableLvalue(CType *ty);

bool typeSame(CType *ty1, CType *ty2);

CType *commonRealCType(CType *ty1, CType *ty2);

void printCType(CType *ty, int indent);

#endif
