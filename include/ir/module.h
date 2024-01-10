#ifndef LAROC_IR_MODULE_H
#define LAROC_IR_MODULE_H

#include "typedef.h"

/// An IR module.
struct Module {
  IRFunc **funcs;
};

void printModule(Module *mod);

#endif
