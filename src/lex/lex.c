#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/comment.h"
#include "lex/token.h"

/// scanPosition count characters in [begin, begin+n) and updates \p lineno and
/// \p col .
static void scanPosition(const char *begin, int n, int *lineno, int *col);

Token *lex(const char *source, int len) {
  const char *p = source;
  Token *result = NULL;
  int n;

  // The line and column number of the current position.
  int lineno = 1;
  int col = 1;

  while (p < source + len) {
    // Skip whitespaces.
    if (isspace(*p)) {
      scanPosition(p, 1, &lineno, &col);
      p++;
      continue;
    }

    // Skip comments.
    n = scanComment(p, source + len);
    if (n != 0) {
      scanPosition(p, n, &lineno, &col);
      p += n;
      continue;
    }

    Token tok;
    memset(&tok, 0, sizeof(Token));
    if ((n = scanToken(p, source + len, &tok)) == 0) {
      printf("unrecognized character %c\n", *p);
      exit(1);
    }
    tok.lineno = lineno;
    tok.col = col;
    scanPosition(p, n, &lineno, &col);
    p += n;
    arrput(result, tok);
  }

  // Append an EOF token to result.
  Token eof;
  memset(&eof, 0, sizeof(Token));
  eof.lineno = lineno;
  eof.col = col;
  arrput(result, eof);
  return result;
}

static void scanPosition(const char *begin, int n, int *lineno, int *col) {
  for (const char *p = begin; p < begin + n; p++) {
    if (*p == '\n') {
      *lineno += 1;
      *col = 1;
    } else {
      *col += 1;
    }
  }
}
