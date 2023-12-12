#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "lex/num.h"

int main() {
  const char *begin, *end;
  uint64_t x;

  begin = "0";
  end = begin + strlen(begin);
  assert(scanNum(begin, end, &x) == 1);
  assert(x == 0);

  begin = "123";
  end = begin + strlen(begin);
  assert(scanNum(begin, end, &x) == 3);
  assert(x == 123);

  return 0;
};
