#ifndef LAROC_IRGEN_TYPE_H
#define LAROC_IRGEN_TYPE_H

#include "ir/type.h"
#include "sema/type.h"

IRType *newIRTypeFromCType(CType *cty);

#endif
