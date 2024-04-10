#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int scanComment(const char *begin, const char *end) {
  const char *p = begin;
  if (*p != '/') {
    return 0;
  }
  if (++p >= end) {
    return 0;
  }

  // A "//" comment.
  if (*p == '/') {
    do {
      p++;
    } while (p < end && *p != '\n');
    return p - begin;
  }

  // A "/*" comment.
  if (*p == '*') {
    p++;
    while (p + 1 < end) {
      if (*p == '*' && *(p + 1) == '/') {
        p += 2;
        return p - begin;
      }
      p++;
    }
    if (p + 1 >= end) {
      printf("unclosed /* comment");
      exit(1);
    }
  }

  return 0;
}
