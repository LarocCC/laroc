#ifndef LAROC_RISCV_MODULE_H
#define LAROC_RISCV_MODULE_H

#include <stdbool.h>
#include <stdio.h>

#include "typedef.h"

/// An RISC-V object file.
struct ObjectFile {
  RVFunc **funcs;
};

void printObjectFile(FILE *fp, ObjectFile *objFile, bool debug);

#endif
