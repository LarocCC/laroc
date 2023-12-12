#ifndef LAROC_LEX_KWD_H
#define LAROC_LEX_KWD_H

#include "typedef.h"

enum Kwd {
  KWD_INVAL,

  KWD_INT,
  KWD_RETURN,

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
