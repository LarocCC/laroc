#ifndef LAROC_PARSE_TYPE_H
#define LAROC_PARSE_TYPE_H

#include "typedef.h"

/// Parse a list of specifiers start at \p begin, store the parsed specifiers as
/// a type in \p ty, and return the tokens consumed.
int parseSpecifier(ParseCtx *ctx, const Token *begin, CType *ty);

/// Recursively fill TYPE_UNTYPED in \p root with \p val.
CType *fillUntyped(CType *root, CType *val);

#endif
