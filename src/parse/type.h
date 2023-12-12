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

/// Parse a list of specifiers start at \p begin, return the tokens consumed.
int parseSpecifier(const Token *begin, Type *ty);

#endif
