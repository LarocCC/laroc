#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/token.h"

Token *lex(const char *source, int len) {
  const char *p = source;
  Token *result = NULL;
  int n;

  while (p < source + len) {
    // Skip whitespaces.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // TODO: Support comments.

    Token tok;
    memset(&tok, 0, sizeof(Token));
    if ((n = scanToken(p, source + len, &tok)) == 0) {
      printf("unrecognized character %c\n", *p);
      exit(1);
    }
    p += n;
    arrput(result, tok);
  }

  // Append an EOF token to result.
  Token eof;
  memset(&eof, 0, sizeof(Token));
  arrput(result, eof);
  return result;
}
