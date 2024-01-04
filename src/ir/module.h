#ifndef LAROC_IR_MODULE_H
#define LAROC_IR_MODULE_H

#include "typedef.h"

struct Module {
  IRFunc **funcs;
};

void printModule(Module *mod);

#endif
