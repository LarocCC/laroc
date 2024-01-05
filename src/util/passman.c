#include <stdlib.h>

#include "stb/stb_ds.h"

#include "ir/module.h"
#include "riscv/objfile.h"
#include "typedef.h"
#include "util/passman.h"

typedef struct IRPass {
  const char *name;
  IRPassRunner *runner;
} IRPass;

typedef struct RVPass {
  const char *name;
  RVPassRunner *runner;
} RVPass;

static IRPass *irPasses = NULL;
static RVPass *rvPasses = NULL;

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

void runAllIRPass(Module *mod, const char *printAfter) {
  for (int i = 0; i < arrlen(irPasses); i++) {
    irPasses[i].runner(mod);

    if (strcmp(irPasses[i].name, printAfter) == 0)
      printModule(mod);
  }
}

void runAllRVPass(ObjectFile *objFile, const char *printAfter) {
  for (int i = 0; i < arrlen(rvPasses); i++) {
    rvPasses[i].runner(objFile);

    if (strcmp(rvPasses[i].name, printAfter) == 0)
      printObjectFile(objFile, true);
  }
}
