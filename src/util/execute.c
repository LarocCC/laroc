#include <assert.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void execute(char *const argv[]) {
  pid_t pid;
  int err = posix_spawnp(&pid, argv[0], NULL, NULL, argv, __environ);
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
