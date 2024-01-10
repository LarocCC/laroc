#ifndef LAROC_UTIL_ARGPARSE_H
#define LAROC_UTIL_ARGPARSE_H

#include <stdbool.h>

#include "typedef.h"

struct CLIOpt {
  const char **inputs;

  const char *printAfter; // -print-after=
};

CLIOpt *parseArgs(int argc, char *argv[]);

#endif
