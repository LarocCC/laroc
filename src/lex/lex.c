#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/comment.h"
#include "lex/lex.h"
#include "lex/token.h"
#include "util/diag.h"

Token *lex(const char *source, int len) {
  const char *p = source;
  Token *result = NULL;
  int n;

  LexCtx ctx;
  ctx.p = source;
  ctx.lineno = 1;
  ctx.col = 1;

  while (p < source + len) {
    // Skip whitespaces.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Skip comments.
    n = scanComment(&ctx, p, source + len);
    if (n != 0) {
      p += n;
      continue;
    }

    updateContextTo(&ctx, p);
    SourceLoc *loc = newSourceLoc(ctx.lineno, ctx.col);
    Token tok;
    memset(&tok, 0, sizeof(Token));
    if ((n = scanToken(p, source + len, &tok)) == 0) {
      emitDiagnostic(loc, "Unrecognized character");
    }
    tok.loc = loc;
    p += n;
    arrput(result, tok);
  }

  // Append an EOF token to result.
  Token eof;
  memset(&eof, 0, sizeof(Token));
  updateContextTo(&ctx, p);
  eof.loc = newSourceLoc(ctx.lineno, ctx.col);
  arrput(result, eof);
  return result;
}

void updateContextTo(LexCtx *ctx, const char *newPos) {
  while (ctx->p < newPos) {
    if (*ctx->p == '\n') {
      ctx->lineno += 1;
      ctx->col = 1;
    } else {
      ctx->col += 1;
    }
    ctx->p++;
  }
}
