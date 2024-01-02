#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "util/argparse.h"

CLIOpt *parseArgs(int argc, char *argv[]) {
  CLIOpt *opt = calloc(1, sizeof(CLIOpt));

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      arrput(opt->inputs, argv[i]);
      continue;
    }

    if (strcmp(argv[i], "-print-tokens") == 0) {
      opt->printToken = true;
      continue;
    }
    if (strcmp(argv[i], "-print-ast") == 0) {
      opt->printAST = true;
      continue;
    }
    if (strcmp(argv[i], "-print-ir") == 0) {
      opt->printIR = true;
      continue;
    }
    if (strcmp(argv[i], "-print-dag") == 0) {
      opt->printDAG = true;
      continue;
    }

    printf("unrecognized option %s\n", argv[i]);
    exit(1);
  }

  return opt;
}
