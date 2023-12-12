#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/token.h"

Token *lex(const char *source, int len) {
  const char *p = source;
  Token *result = NULL;

  while (p < source + len) {
    if (isspace(*p)) {
      while (isspace(*p))
        p++;
      continue;
    }

    Token tok;
    memset(&tok, 0, sizeof(Token));
    p += scanToken(p, source + len, &tok);
    arrput(result, tok);
  }

  Token eof;
  memset(&eof, 0, sizeof(Token));
  arrput(result, eof);
  return result;
}
