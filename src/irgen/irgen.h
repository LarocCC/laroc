#ifndef LAROC_IRGEN_IRGEN_H
#define LAROC_IRGEN_IRGEN_H

#include "typedef.h"

struct IRGenCtx {
};

Module *generateIR(TranslationUnit *unit);

#endif
