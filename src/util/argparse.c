#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "util/argparse.h"

CLIOpt *parseArgs(int argc, char *argv[]) {
  CLIOpt *opt = calloc(1, sizeof(CLIOpt));

  const char *printAfterStr = "-print-after=";
  int printAfterStrLen = strlen(printAfterStr);

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      arrput(opt->inputs, argv[i]);
      continue;
    }

    if (strncmp(argv[i], printAfterStr, printAfterStrLen) == 0) {
      opt->printAfter = argv[i] + printAfterStrLen;
      continue;
    }

    printf("unrecognized option %s\n", argv[i]);
    exit(1);
  }

  return opt;
}
