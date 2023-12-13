#include <stdio.h>
#include <string.h>

#include "lex/kwd.h"

Kwd matchKwd(const char *begin, const char *end) {
  for (Kwd k = KWD_INVAL; k < KWD_LEN; k++) {
    if (begin[0] == kwdInfo[k].str[0] && end - begin == kwdInfo[k].strlen) {
      if (memcmp(begin, kwdInfo[k].str, end - begin) == 0)
        return k;
    }
  }
  return KWD_INVAL;
}

void printKwd(Kwd k) { printf("%s", kwdInfo[k].str); }

KwdInfoItem kwdInfo[KWD_LEN] = {
  [KWD_INVAL] = {
    .str = "<@invalid kwd@>",
    .strlen = 0,
  },
  [KWD_AUTO] = {
    .str = "auto",
    .strlen = 4,
  },
  [KWD_BREAK] = {
    .str = "break",
    .strlen = 5,
  },
  [KWD_CASE] = {
    .str = "case",
    .strlen = 4,
  },
  [KWD_CHAR] = {
    .str = "char",
    .strlen = 4,
  },
  [KWD_CONST] = {
    .str = "const",
    .strlen = 5,
  },
  [KWD_CONTINUE] = {
    .str = "continue",
    .strlen = 8,
  },
  [KWD_DEFAULT] = {
    .str = "default",
    .strlen = 7,
  },
  [KWD_DO] = {
    .str = "do",
    .strlen = 2,
  },
  [KWD_DOUBLE] = {
    .str = "double",
    .strlen = 6,
  },
  [KWD_ELSE] = {
    .str = "else",
    .strlen = 4,
  },
  [KWD_ENUM] = {
    .str = "enum",
    .strlen = 4,
  },
  [KWD_EXTERN] = {
    .str = "extern",
    .strlen = 6,
  },
  [KWD_FLOAT] = {
    .str = "float",
    .strlen = 5,
  },
  [KWD_FOR] = {
    .str = "for",
    .strlen = 3,
  },
  [KWD_GOTO] = {
    .str = "goto",
    .strlen = 4,
  },
  [KWD_IF] = {
    .str = "if",
    .strlen = 2,
  },
  [KWD_INLINE] = {
    .str = "inline",
    .strlen = 6,
  },
  [KWD_INT] = {
    .str = "int",
    .strlen = 3,
  },
  [KWD_LONG] = {
    .str = "long",
    .strlen = 4,
  },
  [KWD_REGISTER] = {
    .str = "register",
    .strlen = 8,
  },
  [KWD_RESTRICT] = {
    .str = "restrict",
    .strlen = 8,
  },
  [KWD_RETURN] = {
    .str = "return",
    .strlen = 6,
  },
  [KWD_SHORT] = {
    .str = "short",
    .strlen = 5,
  },
  [KWD_SIGNED] = {
    .str = "signed",
    .strlen = 6,
  },
  [KWD_SIZEOF] = {
    .str = "sizeof",
    .strlen = 6,
  },
  [KWD_STATIC] = {
    .str = "static",
    .strlen = 6,
  },
  [KWD_STRUCT] = {
    .str = "struct",
    .strlen = 6,
  },
  [KWD_SWITCH] = {
    .str = "switch",
    .strlen = 6,
  },
  [KWD_TYPEDEF] = {
    .str = "typedef",
    .strlen = 7,
  },
  [KWD_UNION] = {
    .str = "union",
    .strlen = 5,
  },
  [KWD_UNSIGNED] = {
    .str = "unsigned",
    .strlen = 8,
  },
  [KWD_VOID] = {
    .str = "void",
    .strlen = 4,
  },
  [KWD_VOLATILE] = {
    .str = "volatile",
    .strlen = 8,
  },
  [KWD_WHILE] = {
    .str = "while",
    .strlen = 5,
  },
  [KWD_BOOL] = {
    .str = "_Bool",
    .strlen = 5,
  },
  [KWD_COMPLEX] = {
    .str = "_Complex",
    .strlen = 8,
  },
  [KWD_IMAGINARY] = {
    .str = "_Imaginary",
    .strlen = 10,
  },
};
