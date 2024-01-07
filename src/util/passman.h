#ifndef LAROC_UTIL_PASSMAN_H
#define LAROC_UTIL_PASSMAN_H

#include "typedef.h"

typedef void SemaPassRunner(TranslationUnit *unit);
typedef void IRPassRunner(Module *mod);
typedef void RVPassRunner(ObjectFile *objFile);

void registerSemaPass(const char *name, SemaPassRunner *runner);
void registerIRPass(const char *name, IRPassRunner *runner);
void registerRVPass(const char *name, RVPassRunner *runner);

void runAllSemaPass(TranslationUnit *unit, const char *printAfter);
void runAllIRPass(Module *mod, const char *printAfter);
void runAllRVPass(ObjectFile *objFile, const char *printAfter);

#endif
