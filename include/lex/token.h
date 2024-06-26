#ifndef LAROC_LEX_TOKEN_H
#define LAROC_LEX_TOKEN_H

#include <stdbool.h>

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/punct.h"

typedef enum TokenKind {
  TOK_EOF,
  TOK_KWD,
  TOK_IDENT,
  TOK_NUM,
  TOK_PUNCT,
} TokenKind;

/// A C token.
///
/// TODO: Support character constant and string literal.
/// TODO: Support preprocessing tokens.
struct Token {
  TokenKind kind;

  union {
    Kwd kwd;
    char *ident;
    Number *num;
    Punct punct;
  };

  // Location of the first character in this token.
  SourceLoc *loc;
};

/// Scan a token start from \p begin and end before \p end, store the result to
/// \p tok and return the characters consumed.
int scanToken(LexCtx *ctx, const char *begin, const char *end, Token *tok);

bool tokenIsKwd(const Token *tok, Kwd k);
bool tokenIsPunct(const Token *tok, Punct p);

void printToken(const Token *tok);

#endif
