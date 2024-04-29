#ifndef LAROC_LEX_LEX_H
#define LAROC_LEX_LEX_H

#include "typedef.h"

struct LexCtx {
  // The SourceLocation of the current position.
  const char *p;
  SourceLoc *loc;
};

/// Scan tokens in [source, source+len), return a array of tokens. The last
/// token in the result is TOK_EOF.
Token *lex(const char *source, int len, const char *filename);

/// Update \p ctx to \p newPos, set ctx->lineno and ctx->col.
void updateContextTo(LexCtx *ctx, const char *newPos);

#endif
