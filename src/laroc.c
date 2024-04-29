#define _DEFAULT_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
#include "util/argparse.h"
#include "util/execute.h"
#include "util/file.h"
#include "util/passman.h"

int main(int argc, char *argv[]) {
  CLIOpt *opt = parseArgs(argc, argv);

  int len;
  const char *source = readFile(opt->input, &len);

  Token *tokens = lex(source, len, opt->input);
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
    printObjectFile(stdout, objFile, true);
    return 0;
  }

  registerRVPass("liveanalysis", liveVarAnalysis);
  registerRVPass("regalloc", allocRegs);
  registerRVPass("pei", insertPrologueEpilogue);
  runAllRVPass(objFile, opt->printAfter);

  if (opt->printAfter) {
    printf("Pass '%s' does not exists!\n", opt->printAfter);
    return 1;
  }

  if (opt->assemble) {
    FILE *outFile = stdout;
    if (opt->output) {
      outFile = fopen(opt->output, "w");
      if (!outFile) {
        perror("open");
        return 1;
      }
    }
    printObjectFile(outFile, objFile, false);
    return 0;
  }

  if (opt->compile) {
    const char *template = "/tmp/laroc-XXXXXX.s";
    char *filename = malloc(strlen(template));
    strcpy(filename, template);
    int fd = mkstemps(filename, 2);
    FILE *tmp = fdopen(fd, "w");
    printObjectFile(tmp, objFile, false);
    fclose(tmp);

    assemble(filename, opt->output);
    return 0;
  }

  assert(false && "Don't know how to compile with these flags.");
}
