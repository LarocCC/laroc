#ifndef LAROC_LEX_LEX_H
#define LAROC_LEX_LEX_H

#include "typedef.h"

struct LexCtx {
  // The line and column number of the current position.
  const char *p;
  int lineno, col;
};

/// Scan tokens in [source, source+len), return a array of tokens. The last
/// token in the result is TOK_EOF.
Token *lex(const char *source, int len);

/// Update \p ctx to \p newPos, set ctx->lineno and ctx->col.
void updateContextTo(LexCtx *ctx, const char *newPos);

#endif
