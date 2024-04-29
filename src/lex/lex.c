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

Token *lex(const char *source, int len, const char *filename) {
  const char *p = source;
  Token *result = NULL;
  int n;

  LexCtx ctx;
  ctx.p = source;
  ctx.loc = newSourceLoc(filename, 1, 1);

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
    Token tok;
    memset(&tok, 0, sizeof(Token));
    if ((n = scanToken(&ctx, p, source + len, &tok)) == 0) {
      emitDiagnostic(ctx.loc, "Unrecognized character");
    }
    tok.loc = copySourceLoc(ctx.loc);
    p += n;
    arrput(result, tok);
  }

  // Append an EOF token to result.
  Token eof;
  memset(&eof, 0, sizeof(Token));
  updateContextTo(&ctx, p);
  eof.loc = copySourceLoc(ctx.loc);
  arrput(result, eof);
  return result;
}

void updateContextTo(LexCtx *ctx, const char *newPos) {
  while (ctx->p < newPos) {
    if (*ctx->p == '\n') {
      ctx->loc->lineno += 1;
      ctx->loc->col = 1;
    } else {
      ctx->loc->col += 1;
    }
    ctx->p++;
  }
}
