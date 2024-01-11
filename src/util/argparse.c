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

    if (!strcmp(argv[i], "-o")) {
      if (i + 1 == argc) {
        printf("Missing output filename");
        exit(1);
      }
      opt->output = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-E") || !strcmp(argv[i], "--preprocess")) {
      opt->preprocess = true;
      continue;
    }
    if (!strcmp(argv[i], "-S") || !strcmp(argv[i], "--assemble")) {
      opt->assemble = true;
      continue;
    }
    if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--compile")) {
      opt->compile = true;
      continue;
    }

    const char *printAfterStr = "-print-after=";
    int printAfterStrLen = strlen(printAfterStr);
    if (strncmp(argv[i], printAfterStr, printAfterStrLen) == 0) {
      opt->printAfter = argv[i] + printAfterStrLen;
      continue;
    }

    printf("unrecognized option %s\n", argv[i]);
    exit(1);
  }

  return opt;
}
