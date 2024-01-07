#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/module.h"
#include "riscv/objfile.h"
#include "sema/transunit.h"
#include "util/passman.h"

typedef struct SemaPass {
  const char *name;
  SemaPassRunner *runner;
} SemaPass;

typedef struct IRPass {
  const char *name;
  IRPassRunner *runner;
} IRPass;

typedef struct RVPass {
  const char *name;
  RVPassRunner *runner;
} RVPass;

static SemaPass *semaPasses = NULL;
static IRPass *irPasses = NULL;
static RVPass *rvPasses = NULL;

void registerSemaPass(const char *name, SemaPassRunner *runner) {
  SemaPass pass = {
      .name = name,
      .runner = runner,
  };
  arrput(semaPasses, pass);
}

void registerIRPass(const char *name, IRPassRunner *runner) {
  IRPass pass = {
      .name = name,
      .runner = runner,
  };
  arrput(irPasses, pass);
}

void registerRVPass(const char *name, RVPassRunner *runner) {
  RVPass pass = {
      .name = name,
      .runner = runner,
  };
  arrput(rvPasses, pass);
}

void runAllSemaPass(TranslationUnit *unit, const char *printAfter) {
  for (int i = 0; i < arrlen(semaPasses); i++) {
    semaPasses[i].runner(unit);

    if (strcmp(semaPasses[i].name, printAfter) == 0) {
      printTranslationUnit(unit);
      exit(0);
    }
  }
}

void runAllIRPass(Module *mod, const char *printAfter) {
  for (int i = 0; i < arrlen(irPasses); i++) {
    irPasses[i].runner(mod);

    if (strcmp(irPasses[i].name, printAfter) == 0) {
      printModule(mod);
      exit(0);
    }
  }
}

void runAllRVPass(ObjectFile *objFile, const char *printAfter) {
  for (int i = 0; i < arrlen(rvPasses); i++) {
    rvPasses[i].runner(objFile);

    if (strcmp(rvPasses[i].name, printAfter) == 0) {
      printObjectFile(objFile, true);
      exit(0);
    }
  }
}
