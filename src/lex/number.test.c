#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "lex/number.h"

int main() {
  const char *begin, *end;
  Number num;

  begin = "0";
  end = begin + strlen(begin);
  assert(scanNumber(begin, end, &num) == 1);
  assert(num.x == 0);

  begin = "123";
  end = begin + strlen(begin);
  assert(scanNumber(begin, end, &num) == 3);
  assert(num.x == 123);

  return 0;
};
