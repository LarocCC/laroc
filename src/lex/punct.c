#include <stdio.h>

#include "lex/punct.h"

Punct scanPunct(const char *begin, const char *end) {
  switch (*begin) {
  case '[':
    return PUNCT_BRACKET_L;
  case ']':
    return PUNCT_BRACKET_R;
  case '(':
    return PUNCT_PAREN_L;
  case ')':
    return PUNCT_PAREN_R;
  case '{':
    return PUNCT_BRACE_L;
  case '}':
    return PUNCT_BRACE_R;
  case '.':
    if (end - begin >= 3 && begin[1] == '.' && begin[2] == '.')
      return PUNCT_ELLIPSIS;
    return PUNCT_DOT;

  case '&':
    if (end - begin >= 2) {
      if (begin[1] == '&')
        return PUNCT_LOGIC_AND;
      if (begin[1] == '=')
        return PUNCT_BIT_AND_EQ;
    }
    return PUNCT_AMP;
  case '*':
    if (end - begin >= 2 && begin[1] == '=')
      return PUNCT_MUL_EQ;
    return PUNCT_STAR;
  case '+':
    if (end - begin >= 2) {
      if (begin[1] == '+')
        return PUNCT_INCR;
      if (begin[1] == '=')
        return PUNCT_ADD_EQ;
    }
    return PUNCT_PLUS;
  case '-':
    if (end - begin >= 2) {
      if (begin[1] == '>')
        return PUNCT_ARROW;
      if (begin[1] == '-')
        return PUNCT_DECR;
      if (begin[1] == '=')
        return PUNCT_SUB_EQ;
    }
    return PUNCT_MINUS;
  case '~':
    return PUNCT_BIT_NOT;
  case '!':
    if (end - begin >= 2 && begin[1] == '=')
      return PUNCT_NEQ;
    return PUNCT_LOGIC_NOT;

  case '<':
    if (end - begin >= 2) {
      if (begin[1] == '=')
        return PUNCT_LEQ;
      if (begin[1] == '<') {
        if (end - begin >= 3 && begin[2] == '=')
          return PUNCT_SHIFT_L_EQ;
        return PUNCT_SHIFT_L;
      }
      if (begin[1] == ':') // digraph
        return PUNCT_BRACKET_L;
      if (begin[1] == '%') // digraph
        return PUNCT_BRACE_L;
    }
    return PUNCT_LT;
  case '>':
    if (end - begin >= 2) {
      if (begin[1] == '=')
        return PUNCT_GEQ;
      if (begin[1] == '>') {
        if (end - begin >= 3 && begin[2] == '=')
          return PUNCT_SHIFT_R_EQ;
        return PUNCT_SHIFT_R;
      }
      if (begin[1] == ':') // digraph
        return PUNCT_BRACKET_R;
    }
    return PUNCT_GT;
  case '^':
    if (end - begin >= 2 && begin[1] == '=')
      return PUNCT_BIT_XOR_EQ;
    return PUNCT_BIT_XOR;
  case '|':
    if (end - begin >= 2) {
      if (begin[1] == '|')
        return PUNCT_LOGIC_OR;
      if (begin[1] == '=')
        return PUNCT_BIT_OR_EQ;
    }
    return PUNCT_BIT_OR;

  case '?':
    return PUNCT_COND;
  case ':':
    if (end - begin >= 2 && begin[1] == '>')
      return PUNCT_BRACKET_R;
    return PUNCT_COLON;
  case ';':
    return PUNCT_SEMICOLON;

  case '/':
    if (end - begin >= 2 && begin[1] == '=')
      return PUNCT_DIV_EQ;
    return PUNCT_DIV;
  case '%':
    if (end - begin >= 2) {
      if (begin[1] == '=')
        return PUNCT_MOD_EQ;
      if (begin[1] == '>') // digraph
        return PUNCT_BRACE_R;
      if (begin[1] == ':') { // digraph
        if (end - begin >= 4 && begin[2] == '%' && begin[3] == ':')
          return PUNCT_HASHHASH;
        return PUNCT_HASH;
      }
    }
    return PUNCT_MOD;

  case '=':
    if (end - begin >= 2 && begin[1] == '=')
      return PUNCT_EQ_CMP;
    return PUNCT_EQ_ASSIGN;

  case ',':
    return PUNCT_COMMA;
  case '#':
    if (end - begin >= 2 && begin[1] == '#')
      return PUNCT_HASHHASH;
    return PUNCT_HASH;

  default:
    return PUNCT_INVAL;
  }
  return 0;
}

PunctInfoItem punctInfo[PUNCT_LEN] = {
  [PUNCT_INVAL] = {
    .str = "<@inval punct@>",
    .strlen = 0,
  },
  [PUNCT_BRACKET_L] = {
    .str = "[",
    .strlen = 1,
  },
  [PUNCT_BRACKET_R] = {
    .str = "]",
    .strlen = 1,
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
  [PUNCT_DOT] = {
    .str = ".",
    .strlen = 1,
  },
  [PUNCT_ARROW] = {
    .str = "->",
    .strlen = 2,
  },
  [PUNCT_INCR] = {
    .str = "++",
    .strlen = 2,
  },
  [PUNCT_DECR] = {
    .str = "--",
    .strlen = 2,
  },
  [PUNCT_AMP] = {
    .str = "&",
    .strlen = 1,
  },
  [PUNCT_STAR] = {
    .str = "*",
    .strlen = 1,
  },
  [PUNCT_PLUS] = {
    .str = "+",
    .strlen = 1,
  },
  [PUNCT_MINUS] = {
    .str = "-",
    .strlen = 1,
  },
  [PUNCT_BIT_NOT] = {
    .str = "~",
    .strlen = 1,
  },
  [PUNCT_LOGIC_NOT] = {
    .str = "!",
    .strlen = 1,
  },
  [PUNCT_LT] = {
    .str = "<",
    .strlen = 1,
  },
  [PUNCT_GT] = {
    .str = ">",
    .strlen = 1,
  },
  [PUNCT_LEQ] = {
    .str = "<=",
    .strlen = 2,
  },
  [PUNCT_GEQ] = {
    .str = ">=",
    .strlen = 2,
  },
  [PUNCT_EQ_CMP] = {
    .str = "==",
    .strlen = 2,
  },
  [PUNCT_NEQ] = {
    .str = "!=",
    .strlen = 2,
  },
  [PUNCT_BIT_XOR] = {
    .str = "^",
    .strlen = 1,
  },
  [PUNCT_BIT_OR] = {
    .str = "|",
    .strlen = 1,
  },
  [PUNCT_LOGIC_AND] = {
    .str = "&&",
    .strlen = 2,
  },
  [PUNCT_LOGIC_OR] = {
    .str = "||",
    .strlen = 2,
  },
  [PUNCT_COND] = {
    .str = "?",
    .strlen = 1,
  },
  [PUNCT_COLON] = {
    .str = ":",
    .strlen = 1,
  },
  [PUNCT_SEMICOLON] = {
    .str = ";",
    .strlen = 1,
  },
  [PUNCT_ELLIPSIS] = {
    .str = "...",
    .strlen = 3,
  },
  [PUNCT_DIV] = {
    .str = "/",
    .strlen = 1,
  },
  [PUNCT_MOD] = {
    .str = "%",
    .strlen = 1,
  },
  [PUNCT_SHIFT_L] = {
    .str = "<<",
    .strlen = 2,
  },
  [PUNCT_SHIFT_R] = {
    .str = ">>",
    .strlen = 2,
  },
  [PUNCT_EQ_ASSIGN] = {
    .str = "=",
    .strlen = 1,
  },
  [PUNCT_MUL_EQ] = {
    .str = "*=",
    .strlen = 2,
  },
  [PUNCT_DIV_EQ] = {
    .str = "/=",
    .strlen = 2,
  },
  [PUNCT_MOD_EQ] = {
    .str = "%=",
    .strlen = 2,
  },
  [PUNCT_ADD_EQ] = {
    .str = "+=",
    .strlen = 2,
  },
  [PUNCT_SUB_EQ] = {
    .str = "-=",
    .strlen = 2,
  },
  [PUNCT_SHIFT_L_EQ] = {
    .str = "<<=",
    .strlen = 3,
  },
  [PUNCT_SHIFT_R_EQ] = {
    .str = ">>=",
    .strlen = 3,
  },
  [PUNCT_BIT_AND_EQ] = {
    .str = "&=",
    .strlen = 2,
  },
  [PUNCT_BIT_XOR_EQ] = {
    .str = "^=",
    .strlen = 2,
  },
  [PUNCT_BIT_OR_EQ] = {
    .str = "|=",
    .strlen = 2,
  },
  [PUNCT_COMMA] = {
    .str = ",",
    .strlen = 1,
  },
  [PUNCT_HASH] = {
    .str = "#",
    .strlen = 1,
  },
  [PUNCT_HASHHASH] = {
    .str = "##",
    .strlen = 2,
  },
};
