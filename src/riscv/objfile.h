#ifndef LAROC_RISCV_MODULE_H
#define LAROC_RISCV_MODULE_H

#include "typedef.h"

struct ObjectFile {
  RVFunc **funcs;
};

void printObjectFile(ObjectFile *objFile);

#endif
