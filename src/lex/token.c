#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lex/kwd.h"
#include "lex/num.h"
#include "lex/punct.h"
#include "lex/token.h"

int scanToken(const char *begin, Token *tok) {
  if (isalpha(*begin) || *begin == '_') {
    const char *p = begin;
    while (isalpha(*p) || *p == '_')
      p++;

    if ((tok->kwd = matchKwd(begin, p)) != KWD_INVAL) {
      tok->kind = TOK_KWD;
      return p - begin;
    }

    int len = p - begin;
    tok->kind = TOK_IDENT;
    tok->ident = malloc(len + 1);
    memcpy(tok->ident, begin, len);
    tok->ident[len] = '\0';
    return p - begin;
  }

  if (isdigit(*begin)) {
    tok->kind = TOK_NUM;
    return scanNum(begin, &tok->num);
  }

  if ((tok->punct = scanPunct(begin)) != PUNCT_INVAL) {
    tok->kind = TOK_PUNCT;
    return punctInfo[tok->punct].strlen;
  }

  tok->kind = TOK_INVAL;
  return 0;
}
