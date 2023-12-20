#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

const char *readFile(const char *filename, int *len) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(1);
  }

  *len = lseek(fd, 0, SEEK_END);
  if (*len == -1) {
    perror("lseek");
    exit(1);
  }

  const char *content = mmap(NULL, *len, PROT_READ, MAP_SHARED, fd, 0);
  if (content == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  return content;
}
