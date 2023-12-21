#ifndef LAROC_LEX_PUNCT_H
#define LAROC_LEX_PUNCT_H

#include "typedef.h"

enum Punct {
  PUNCT_INVAL,

  PUNCT_BRACKET_L, // [
  PUNCT_BRACKET_R, // ]
  PUNCT_PAREN_L,   // (
  PUNCT_PAREN_R,   // )
  PUNCT_BRACE_L,   // {
  PUNCT_BRACE_R,   // }
  PUNCT_DOT,       // .
  PUNCT_ARROW,     // ->

  PUNCT_INCR,      // ++
  PUNCT_DECR,      // --
  PUNCT_AMP,       // &
  PUNCT_STAR,      // *
  PUNCT_PLUS,      // +
  PUNCT_MINUS,     // -
  PUNCT_BIT_NOT,   // ~
  PUNCT_LOGIC_NOT, // !

  PUNCT_LT,        // <
  PUNCT_GT,        // >
  PUNCT_LEQ,       // <=
  PUNCT_GEQ,       // >=
  PUNCT_EQ_CMP,    // ==
  PUNCT_NEQ,       // !=
  PUNCT_BIT_XOR,   // ^
  PUNCT_BIT_OR,    // |
  PUNCT_LOGIC_AND, // &&
  PUNCT_LOGIC_OR,  // ||

  PUNCT_COND,      // ?
  PUNCT_COLON,     // :
  PUNCT_SEMICOLON, // ;
  PUNCT_ELLIPSIS,  // ...

  PUNCT_DIV,     // /
  PUNCT_MOD,     // %
  PUNCT_SHIFT_L, // <<
  PUNCT_SHIFT_R, // >>

  PUNCT_EQ_ASSIGN,  // =
  PUNCT_MUL_EQ,     // *=
  PUNCT_DIV_EQ,     // /=
  PUNCT_MOD_EQ,     // %=
  PUNCT_ADD_EQ,     // +=
  PUNCT_SUB_EQ,     // -=
  PUNCT_SHIFT_L_EQ, // <<=
  PUNCT_SHIFT_R_EQ, // >>=
  PUNCT_BIT_AND_EQ, // &=
  PUNCT_BIT_XOR_EQ, // ^=
  PUNCT_BIT_OR_EQ,  // |=

  PUNCT_COMMA,    // ,
  PUNCT_HASH,     // #
  PUNCT_HASHHASH, // ##

  PUNCT_DIGRAPH_BRACKET_L, // <:
  PUNCT_DIGRAPH_BRACKET_R, // :>
  PUNCT_DIGRAPH_BRACE_L,   // <%
  PUNCT_DIGRAPH_BRACE_R,   // %>
  PUNCT_DIGRAPH_HASH,      // %:
  PUNCT_DIGRAPH_HASHHASH,  // %:%:

  PUNCT_LEN,
};

/// Scan a punctuator start from \p begin and end before \p end.
Punct scanPunct(const char *begin, const char *end);

typedef struct PunctInfoItem {
  const char *str;
  unsigned strlen;
} PunctInfoItem;

extern PunctInfoItem punctInfo[PUNCT_LEN];

#endif
