#ifndef LAROC_LEX_PUNCT_H
#define LAROC_LEX_PUNCT_H

#include "typedef.h"

enum Punct {
  PUNCT_INVAL,

  PUNCT_PAREN_L, // (
  PUNCT_PAREN_R, // )
  PUNCT_BRACE_L, // {
  PUNCT_BRACE_R, // }

  PUNCT_SEMICOLON, // ;

  PUNCT_LEN,
};

/// Scan a punctuator start from \p p.
Punct scanPunct(const char *p);

typedef struct PunctInfoItem {
  const char *str;
  unsigned strlen;
} PunctInfoItem;

extern PunctInfoItem punctInfo[PUNCT_LEN];

#endif
