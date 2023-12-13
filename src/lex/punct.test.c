#include <assert.h>
#include <string.h>

#include "lex/punct.h"

int main() {
  const char *begin, *end;

  for (Punct p = PUNCT_INVAL; p < PUNCT_LEN; p++) {
    assert(punctInfo[p].str != 0);
    if (p == PUNCT_INVAL)
      continue;

    begin = punctInfo[p].str;
    end = begin + punctInfo[p].strlen;
    assert(scanPunct(begin, end) == p);
  }

  begin = "<:";
  end = begin + strlen(begin);
  assert(scanPunct(begin, end) == PUNCT_BRACKET_L);

  begin = ":>";
  end = begin + strlen(begin);
  assert(scanPunct(begin, end) == PUNCT_BRACKET_R);

  begin = "<%";
  end = begin + strlen(begin);
  assert(scanPunct(begin, end) == PUNCT_BRACE_L);

  begin = "%>";
  end = begin + strlen(begin);
  assert(scanPunct(begin, end) == PUNCT_BRACE_R);

  begin = "%:";
  end = begin + strlen(begin);
  assert(scanPunct(begin, end) == PUNCT_HASH);

  begin = "%:%:";
  end = begin + strlen(begin);
  assert(scanPunct(begin, end) == PUNCT_HASHHASH);
  return 0;
}
