#ifndef LAROC_PARSE_DECL_H
#define LAROC_PARSE_DECL_H

#include "typedef.h"

int parseDeclaration(ParseCtx *ctx, const Token *begin, Declaration *decltion);

int parseTypeName(ParseCtx *ctx, const Token *begin, CType **ty);

#endif
