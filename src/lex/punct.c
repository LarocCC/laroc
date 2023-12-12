#include <stdio.h>

#include "lex/punct.h"

Punct scanPunct(const char *begin, const char *end) {
  switch (*begin) {
  case '(':
    return PUNCT_PAREN_L;
  case ')':
    return PUNCT_PAREN_R;
  case '{':
    return PUNCT_BRACE_L;
  case '}':
    return PUNCT_BRACE_R;

  case ';':
    return PUNCT_SEMICOLON;

  default:
    return PUNCT_INVAL;
  }
}

void printPunct(Punct p) { printf("%s", punctInfo[p].str); }

PunctInfoItem punctInfo[PUNCT_LEN] = {
  [PUNCT_INVAL] = {
    .str = "<@inval punct@>",
    .strlen = 0,
  },
  [PUNCT_PAREN_L] = {
    .str = "(",
    .strlen = 1,
  },
  [PUNCT_PAREN_R] = {
    .str = ")",
    .strlen = 1,
  },
  [PUNCT_BRACE_L] = {
    .str = "{",
    .strlen = 1,
  },
  [PUNCT_BRACE_R] = {
    .str = "}",
    .strlen = 1,
  },
  [PUNCT_SEMICOLON] = {
    .str = ";",
    .strlen = 1,
  },
};
