#include <features.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

int main() {
  struct utsname buf;
  if (uname(&buf)) {
    perror("uname");
    return 1;
  }

  printf("Hello laroc! with %s and "
#ifndef __GLIBC__
    "any-possible-libc-but-"
#endif
    "glibc.\n"
  , buf.machine);
  return 0;
}
