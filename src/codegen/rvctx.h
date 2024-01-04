#ifndef LAROC_CODEGEN_RVCTX_H
#define LAROC_CODEGEN_RVCTX_H

#include "typedef.h"

struct RVCtx {
  ObjectFile *objFile;
};

RVCtx *newRVCtx(ObjectFile *objFile);

#endif
