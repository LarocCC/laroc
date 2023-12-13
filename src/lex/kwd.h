#ifndef LAROC_LEX_KWD_H
#define LAROC_LEX_KWD_H

#include "typedef.h"

enum Kwd {
  KWD_INVAL,

  KWD_AUTO,

  KWD_BREAK,

  KWD_CASE,
  KWD_CHAR,
  KWD_CONST,
  KWD_CONTINUE,

  KWD_DEFAULT,
  KWD_DO,
  KWD_DOUBLE,

  KWD_ELSE,
  KWD_ENUM,
  KWD_EXTERN,

  KWD_FLOAT,
  KWD_FOR,

  KWD_GOTO,

  KWD_IF,
  KWD_INLINE,
  KWD_INT,

  KWD_LONG,

  KWD_REGISTER,
  KWD_RESTRICT,
  KWD_RETURN,

  KWD_SHORT,
  KWD_SIGNED,
  KWD_SIZEOF,
  KWD_STATIC,
  KWD_STRUCT,
  KWD_SWITCH,

  KWD_TYPEDEF,

  KWD_UNION,

  KWD_UNSIGNED,

  KWD_VOID,
  KWD_VOLATILE,

  KWD_WHILE,

  KWD_BOOL,      // _Bool
  KWD_COMPLEX,   // _Complex
  KWD_IMAGINARY, // _Imaginary

  KWD_LEN,
};

/// Match a keyword at [begin, end).
Kwd matchKwd(const char *begin, const char *end);

void printKwd(Kwd k);

typedef struct KwdInfoItem {
  const char *str;
  unsigned strlen;
} KwdInfoItem;

extern KwdInfoItem kwdInfo[KWD_LEN];

#endif
