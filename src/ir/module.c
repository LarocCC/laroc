#include <stdio.h>

#include "stb/stb_ds.h"

#include "ir/func.h"
#include "ir/module.h"

void printModule(Module *mod) {
  for (int i = 0; i < arrlen(mod->funcs); i++) {
    if (i != 0)
      printf("\n");
    printIRFunc(mod->funcs[i]);
  }
}
