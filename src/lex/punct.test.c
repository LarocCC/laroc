#include <assert.h>

#include "lex/punct.h"

int main() {
  for (Punct p = PUNCT_INVAL; p < PUNCT_LEN; p++) {
    const char *begin = punctInfo[p].str;
    const char *end = begin + punctInfo[p].strlen;
    assert(scanPunct(begin, end) == p);
  }
  return 0;
}
