#include <stdio.h>
#include <string.h>

#include "lex/kwd.h"

Kwd matchKwd(const char *begin, const char *end) {
  for (Kwd k = KWD_INVAL; k < KWD_LEN; k++) {
    if (end - begin == kwdInfo[k].strlen) {
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
  [KWD_INT] = {
    .str = "int",
    .strlen = 3,
  },
  [KWD_RETURN] = {
    .str = "return",
    .strlen = 6,
  },
};
