#ifndef LAROC_PARSE_TYPE_H
#define LAROC_PARSE_TYPE_H

#include <stdbool.h>

#include "typedef.h"

enum TypeKind {
  TYPE_UNTYPED,

  TYPE_INT,

  TYPE_FUNC,
};

struct Type {
  TypeKind kind;

  struct {
    Type *ret;
    Declarator **params;
  } func;
};

Type *newType(TypeKind kind);

/// Parse a list of specifiers start at \p begin, return the tokens consumed.
int parseSpecifier(const Token *begin, Type *ty);

/// Recursively fill TYPE_UNTYPED in \p root with \p val.
Type *fillUntyped(Type *root, Type *val);

bool typeIsInteger(Type *ty);
bool typeIsReal(Type *ty);
bool typeIsArithmetic(Type *ty);

bool typeSame(Type *ty1, Type *ty2);

Type *commonRealType(Type *ty1, Type *ty2);

#endif
