#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/isel.h"
#include "codegen/liveanalysis.h"
#include "codegen/pei.h"
#include "codegen/regalloc.h"
#include "ir/dag.h"
#include "ir/module.h"
#include "irgen/irgen.h"
#include "lex/lex.h"
#include "lex/token.h"
#include "parse/parse.h"
#include "riscv/objfile.h"
#include "sema/labelchk.h"
#include "sema/transunit.h"
#include "sema/typechk.h"
#include "util/argparse.h"
#include "util/file.h"
#include "util/passman.h"

int main(int argc, char *argv[]) {
  CLIOpt *opt = parseArgs(argc, argv);

  bool invalidOpt = false;
  invalidOpt |= arrlen(opt->inputs) != 1;
  invalidOpt |= opt->preprocess;
  invalidOpt |= !opt->assemble;
  invalidOpt |= opt->compile;
  invalidOpt &= opt->printAfter == NULL;
  if (invalidOpt) {
    printf("Usage: %s input.c -S -o output.s\n", argv[0]);
    return 1;
  }

  int len;
  const char *source = readFile(opt->inputs[0], &len);

  Token *tokens = lex(source, len);
  if (opt->printAfter && !strcmp(opt->printAfter, "lex")) {
    for (int i = 0; i < arrlen(tokens); i++)
      printToken(&tokens[i]);
    return 0;
  }

  TranslationUnit *unit = parseTranslationUnit(tokens);
  if (opt->printAfter && !strcmp(opt->printAfter, "parse")) {
    printTranslationUnit(unit);
    return 0;
  }

  registerSemaPass("labelchk", semaLabelCheck);
  registerSemaPass("typechk", semaTypeCheck);
  runAllSemaPass(unit, opt->printAfter);

  Module *mod = genIR(unit);
  if (opt->printAfter && !strcmp(opt->printAfter, "irgen")) {
    printModule(mod);
    return 0;
  }

  registerIRPass("dag", buildDAG);
  runAllIRPass(mod, opt->printAfter);

  ObjectFile *objFile = selectInstruction(mod);
  if (opt->printAfter && !strcmp(opt->printAfter, "isel")) {
    printObjectFile(objFile, true);
    return 0;
  }

  registerRVPass("liveanalysis", liveVarAnalysis);
  registerRVPass("regalloc", allocRegs);
  registerRVPass("pei", insertPrologueEpilogue);
  runAllRVPass(objFile, opt->printAfter);

  printObjectFile(objFile, false);

  return 0;
}
