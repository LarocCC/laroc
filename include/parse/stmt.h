#ifndef LAROC_PARSE_STMT_H
#define LAROC_PARSE_STMT_H

#include "typedef.h"

int parseStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);
int parseCmpdStmt(ParseCtx *ctx, const Token *begin, Stmt *stmt);

#endif
