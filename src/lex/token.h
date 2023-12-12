#ifndef LAROC_LEX_TOKEN_H
#define LAROC_LEX_TOKEN_H

#include <stdbool.h>
#include <stdint.h>

#include "lex/kwd.h"
#include "lex/punct.h"
#include "typedef.h"

enum TokenKind {
  TOK_EOF,
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

/// Scan a token start from \p begin and end before \p end, store the result to
/// \p tok and return the characters consumed.
int scanToken(const char *begin, const char *end, Token *tok);

bool tokenIsKwd(Token *tok, Kwd k);
bool tokenIsPunct(Token *tok, Punct p);

#endif
