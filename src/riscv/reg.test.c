#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "riscv/reg.h"

int main() {
  Reg *a = NULL;
  arrput(a, 1);
  arrput(a, 3);
  arrput(a, 4);

  Reg *b = NULL;
  arrput(b, 1);
  arrput(b, 5);

  Reg *c = mergeRegArr(a, b);
  assert(arrlen(c) == 4);
  assert(c[0] == 1);
  assert(c[1] == 3);
  assert(c[2] == 4);
  assert(c[3] == 5);

  Reg *d = subtractRegArr(a, b);
  assert(arrlen(d) == 2);
  assert(d[0] == 3);
  assert(d[1] == 4);

  return 0;
}
