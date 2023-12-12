#include <assert.h>

#include "lex/kwd.h"

int main() {
  for (Kwd k = KWD_INVAL; k < KWD_LEN; k++) {
    const char *begin = kwdInfo[k].str;
    const char *end = begin + kwdInfo[k].strlen;
    assert(matchKwd(begin, end) == k);
  }
  return 0;
}
