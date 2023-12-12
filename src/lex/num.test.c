#include <assert.h>
#include <stdint.h>

#include "lex/num.h"

int main() {
  uint64_t x;

  assert(scanNum("0", &x) == 1);
  assert(x == 0);

  assert(scanNum("123", &x) == 3);
  assert(x == 123);

  return 0;
};
