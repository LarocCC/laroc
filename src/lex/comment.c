#include <stdbool.h>

#include "typedef.h"
#include "lex/lex.h"
#include "util/diag.h"

int scanComment(LexCtx *ctx, const char *begin, const char *end) {
  const char *p = begin;
  if (*p != '/') {
    return 0;
  }
  if (++p >= end) {
    return 0;
  }

  // A "//" comment.
  if (*p == '/') {
    do {
      p++;
    } while (p < end && *p != '\n');
    return p - begin;
  }

  // A "/*" comment.
  if (*p == '*') {
    p++;
    while (p + 1 < end) {
      if (*p == '*' && *(p + 1) == '/') {
        p += 2;
        return p - begin;
      }
      p++;
    }
    if (p + 1 >= end) {
      updateContextTo(ctx, end);
      emitDiagnostic(ctx->loc, "Unclosed comment");
    }
  }

  return 0;
}
