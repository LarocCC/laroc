#include <assert.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void execute(const char *argv[]) {
  int argc = 0;
  // Incoming argv[argc] should be NULL.
  while (argv[argc])
    argc++;

  // Copy argv to noConstArgv.
  char **noConstArgv = malloc((argc + 1) * sizeof(char *));
  for (int i = 0; i < argc; i++) {
    noConstArgv[i] = malloc(strlen(argv[i]) + 1);
    strcpy(noConstArgv[i], argv[i]);
  }
  noConstArgv[argc] = NULL;

  pid_t pid;
  int err = posix_spawnp(&pid, argv[0], NULL, NULL, noConstArgv, __environ);
  if (err) {
    perror("posix_spawnp");
    exit(1);
  }

  int wstatus;
  waitpid(pid, &wstatus, 0);

  // TODO: Handle other cases.
  assert(WIFEXITED(wstatus));
  if (WEXITSTATUS(wstatus)) {
    printf("subprocess exited with code %d\n", WEXITSTATUS(wstatus));
    exit(1);
  }
}

void assemble(const char *input, const char *output) {
  execute((const char *[]){
    "clang",
    "--target=riscv64",
    "-march=rv64gc",
    input,
    "-c",
    "-o",
    output,
    NULL,
  });
}
