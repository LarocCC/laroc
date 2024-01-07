#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/isel.h"
#include "codegen/liveanalysis.h"
#include "ir/dag.h"
#include "ir/module.h"
#include "irgen/irgen.h"
#include "lex/lex.h"
#include "lex/token.h"
#include "parse/parse.h"
#include "riscv/objfile.h"
#include "sema/transunit.h"
#include "sema/typechk.h"
#include "util/argparse.h"
#include "util/file.h"
#include "util/passman.h"

int main(int argc, char *argv[]) {
  CLIOpt *opt = parseArgs(argc, argv);

  if (arrlen(opt->inputs) != 1) {
    printf("Usage: %s input.c\n", argv[0]);
    return 1;
  }

  int len;
  const char *source = readFile(opt->inputs[0], &len);

  Token *tokens = lex(source, len);
  if (strcmp(opt->printAfter, "lex") == 0) {
    for (int i = 0; i < arrlen(tokens); i++)
      printToken(&tokens[i]);
    return 0;
  }

  TranslationUnit *unit = parseTranslationUnit(tokens);
  if (strcmp(opt->printAfter, "parse") == 0) {
    printTranslationUnit(unit);
    return 0;
  }

  registerSemaPass("typechk", semaTypeCheck);
  runAllSemaPass(unit, opt->printAfter);

  Module *mod = genIR(unit);
  if (strcmp(opt->printAfter, "irgen") == 0) {
    printModule(mod);
    return 0;
  }

  registerIRPass("dag", buildDAG);
  runAllIRPass(mod, opt->printAfter);

  ObjectFile *objFile = selectInstruction(mod);
  if (strcmp(opt->printAfter, "isel") == 0) {
    printObjectFile(objFile, true);
    return 0;
  }

  registerRVPass("liveanalysis", liveVarAnalysis);
  runAllRVPass(objFile, opt->printAfter);

  return 0;
}
