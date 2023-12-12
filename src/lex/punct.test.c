#include <assert.h>

#include "lex/punct.h"

int main() {
  for (Punct p = PUNCT_INVAL; p < PUNCT_LEN; p++)
    assert(scanPunct(punctInfo[p].str) == p);

  return 0;
}
