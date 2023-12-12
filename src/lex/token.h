#ifndef LAROC_LEX_TOKEN_H
#define LAROC_LEX_TOKEN_H

#include <stdint.h>

#include "lex/kwd.h"
#include "lex/punct.h"
#include "typedef.h"

enum TokenKind {
  TOK_INVAL,
  TOK_KWD,
  TOK_IDENT,
  TOK_NUM,
  TOK_PUNCT,
};

struct Token {
  TokenKind kind;

  Kwd kwd;
  char *ident;
  uint64_t num;
  Punct punct;
};

int scanToken(const char *begin, Token *tok);

#endif
