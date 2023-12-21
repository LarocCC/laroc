#ifndef LAROC_IRGEN_DECL_H
#define LAROC_IRGEN_DECL_H

#include "typedef.h"

void genDeclaration(IRGenCtx *ctx, Declaration *decltion);
void genDeclarator(IRGenCtx *ctx, Declarator *decltor);

#endif
