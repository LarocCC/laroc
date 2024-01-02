#ifndef LAROC_PARSE_TYPE_H
#define LAROC_PARSE_TYPE_H

#include <stdbool.h>

#include "typedef.h"

enum CTypeKind {
  TYPE_UNTYPED,

  TYPE_VOID,

  TYPE_INT,

  TYPE_FUNC,
};

enum CTypeAttr {
  TYPE_ATTR_NONE = 0,
  TYPE_ATTR_LVALUE = 1 << 0,
};

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

void computeCTypeSize(CType *ty);

/// Parse a list of specifiers start at \p begin, return the tokens consumed.
int parseSpecifier(const Token *begin, CType *ty);

/// Recursively fill TYPE_UNTYPED in \p root with \p val.
CType *fillUntyped(CType *root, CType *val);

bool typeIsInteger(CType *ty);
bool typeIsReal(CType *ty);
bool typeIsArithmetic(CType *ty);

bool typeIsModifiableLvalue(CType *ty);

bool typeSame(CType *ty1, CType *ty2);

CType *commonRealCType(CType *ty1, CType *ty2);

void printCType(CType *ty, int indent);

#endif
