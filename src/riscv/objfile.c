#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/func.h"
#include "riscv/objfile.h"

void printObjectFile(FILE *fp, ObjectFile *objFile, bool debug) {
  fprintf(fp, "\t.text\n");
  for (int i = 0; i < arrlen(objFile->funcs); i++) {
    if (i != 0)
      fprintf(fp, "\n");
    printRVFunc(fp, objFile->funcs[i], debug);
  }
}
