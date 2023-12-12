#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lex/kwd.h"
#include "lex/num.h"
#include "lex/punct.h"
#include "lex/token.h"

int scanToken(const char *begin, const char *end, Token *tok) {
  if (isalpha(*begin) || *begin == '_') {
    const char *p = begin;
    while (p < end && (isalpha(*p) || *p == '_'))
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
    return scanNum(begin, end, &tok->num);
  }

  if ((tok->punct = scanPunct(begin, end)) != PUNCT_INVAL) {
    tok->kind = TOK_PUNCT;
    return punctInfo[tok->punct].strlen;
  }

  tok->kind = TOK_EOF;
  return 0;
}

bool tokenIsKwd(const Token *tok, Kwd k) {
  return tok->kind == TOK_KWD && tok->kwd == k;
}

bool tokenIsPunct(const Token *tok, Punct p) {
  return tok->kind == TOK_PUNCT && tok->punct == p;
}
