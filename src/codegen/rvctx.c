#include <stdlib.h>

#include "typedef.h"
#include "codegen/rvctx.h"

RVCtx *newRVCtx(ObjectFile *objFile) {
  RVCtx *ctx = calloc(1, sizeof(RVCtx));
  ctx->objFile = objFile;
  return ctx;
}
