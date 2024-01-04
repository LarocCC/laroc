#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/func.h"
#include "riscv/objfile.h"

void printObjectFile(ObjectFile *objFile) {
  for (int i = 0; i < arrlen(objFile->funcs); i++) {
    if (i != 0)
      printf("\n");
    printRVFunc(objFile->funcs[i]);
  }
}
