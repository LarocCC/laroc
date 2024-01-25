#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "util/argparse.h"

static void checkArgs(CLIOpt *opt, const char *argv0);
static const char *inferOutputFilename(CLIOpt *opt);

CLIOpt *parseArgs(int argc, char *argv[]) {
  CLIOpt *opt = calloc(1, sizeof(CLIOpt));

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      assert(!opt->input && "Multiple inputs are not supported yet.");
      opt->input = argv[i];
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

  checkArgs(opt, argv[0]);
  return opt;
}

static void checkArgs(CLIOpt *opt, const char *argv0) {
  if (opt->printAfter)
    return;

  bool invalidOpt = false;

  invalidOpt |= opt->preprocess;
  invalidOpt |= !opt->assemble;
  invalidOpt |= opt->compile;
  if (invalidOpt) {
    printf("Usage: %s input.c -S -o output.s\n", argv0);
    exit(1);
  }

  if (!opt->output)
    opt->output = inferOutputFilename(opt);
}

static const char *inferOutputFilename(CLIOpt *opt) {
  int inputFilenameLen = strlen(opt->input);
  int inputBasenameLen = inputFilenameLen;
  if (!strcmp(&opt->input[inputFilenameLen - 2], ".c"))
    inputBasenameLen -= 2;

  if (opt->preprocess)
    return NULL;

  if (opt->assemble) {
    char *buf = malloc(inputBasenameLen + 3);
    memcpy(buf, opt->input, inputBasenameLen);
    memcpy(buf + inputBasenameLen, ".s", 3);
    return buf;
  }

  if (opt->compile) {
    char *buf = malloc(inputBasenameLen + 3);
    memcpy(buf, opt->input, inputBasenameLen);
    memcpy(buf + inputBasenameLen, ".o", 3);
    return buf;
  }

  return "a.out";
}
