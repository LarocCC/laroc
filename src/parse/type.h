#ifndef LAROC_PARSE_TYPE_H
#define LAROC_PARSE_TYPE_H

#include "typedef.h"

enum TypeKind {
  TYPE_UNTYPED,
  TYPE_INT,
};

struct Type {
  TypeKind kind;
};

Type *newType(TypeKind kind);

/// Parse a list of specifiers start at \p begin, return the tokens consumed.
int parseSpecifier(const Token *begin, Type *ty);

/// Fill TYPE_UNTYPED in \p root with \p val.
Type *fillUntyped(Type *root, Type *val);

#endif
