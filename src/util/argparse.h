#ifndef LAROC_UTIL_ARGPARSE_H
#define LAROC_UTIL_ARGPARSE_H

#include <stdbool.h>

#include "typedef.h"

struct CLIOpt {
  const char **inputs;

  bool printToken; // -print-tokens
  bool printAST;   // -print-ast
  bool printIR;    // -print-ir
};

CLIOpt *parseArgs(int argc, char *argv[]);

#endif
