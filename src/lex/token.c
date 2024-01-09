#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/number.h"
#include "lex/punct.h"
#include "lex/token.h"

int scanToken(const char *begin, const char *end, Token *tok) {
  // A token begin with digit or underscore is a number.
  if (isalpha(*begin) || *begin == '_') {
    const char *p = begin;
    while (p < end && (isalnum(*p) || *p == '_'))
      p++;

    // Is this token a keyword?
    if ((tok->kwd = matchKwd(begin, p)) != KWD_INVAL) {
      tok->kind = TOK_KWD;
      return p - begin;
    }

    // Copy the identifier and return the token.
    int len = p - begin;
    tok->kind = TOK_IDENT;
    tok->ident = malloc(len + 1);
    memcpy(tok->ident, begin, len);
    tok->ident[len] = '\0';
    return p - begin;
  }

  // A token begin with digit is a number.
  if (isdigit(*begin)) {
    tok->kind = TOK_NUM;
    tok->num = calloc(1, sizeof(Number));
    return scanNumber(begin, end, tok->num);
  }

  // Try to scan a punctuator.
  //
  // TODO: Make scanPunct() don't return PUNCT_DIGRAPH_* to simplify code here.
  if ((tok->punct = scanPunct(begin, end)) != PUNCT_INVAL) {
    tok->kind = TOK_PUNCT;
    int n = punctInfo[tok->punct].strlen;

    switch (tok->punct) {
    case PUNCT_DIGRAPH_BRACKET_L:
      tok->punct = PUNCT_BRACKET_L;
      break;
    case PUNCT_DIGRAPH_BRACKET_R:
      tok->punct = PUNCT_BRACKET_R;
      break;
    case PUNCT_DIGRAPH_BRACE_L:
      tok->punct = PUNCT_BRACE_L;
      break;
    case PUNCT_DIGRAPH_BRACE_R:
      tok->punct = PUNCT_BRACE_R;
      break;
    case PUNCT_DIGRAPH_HASHHASH:
      tok->punct = PUNCT_HASHHASH;
      break;
    case PUNCT_DIGRAPH_HASH:
      tok->punct = PUNCT_HASH;
      break;
    default:;
    }

    return n;
  }

  // Not a valid token.
  tok->kind = TOK_EOF;
  return 0;
}

bool tokenIsKwd(const Token *tok, Kwd k) {
  return tok->kind == TOK_KWD && tok->kwd == k;
}

bool tokenIsPunct(const Token *tok, Punct p) {
  return tok->kind == TOK_PUNCT && tok->punct == p;
}

void printToken(const Token *tok) {
  switch (tok->kind) {
  case TOK_EOF:
    printf("EOF\n");
    return;

  case TOK_KWD:
    printf("Keyword '%s'\n", kwdInfo[tok->kwd].str);
    return;

  case TOK_IDENT:
    printf("Ident '%s'\n", tok->ident);
    return;

  case TOK_NUM:
    printf("Number ");
    printNumber(tok->num);
    printf("\n");
    return;

  case TOK_PUNCT:
    printf("Punct '%s'\n", punctInfo[tok->punct].str);
    return;

  default:
    assert(false);
  }
}
