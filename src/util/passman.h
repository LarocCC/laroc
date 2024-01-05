#ifndef LAROC_UTIL_PASSMAN_H
#define LAROC_UTIL_PASSMAN_H

#include "typedef.h"

typedef void IRPassRunner(Module *mod);
typedef void RVPassRunner(ObjectFile *objFile);

void registerIRPass(const char *name, IRPassRunner *runner);
void registerRVPass(const char *name, RVPassRunner *runner);

void runAllIRPass(Module *mod, const char *printAfter);
void runAllRVPass(ObjectFile *objFile, const char *printAfter);

#endif
