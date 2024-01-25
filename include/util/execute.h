#ifndef LAROC_UTIL_EXECUTE
#define LAROC_UTIL_EXECUTE

/// Execute a command via posix_spawnp(). Calls exit(1) if the subprocess
/// doesn't exits with code 0. argv[0] may not be the fullpath of the program.
/// posix_spawnp() will search the program in PATH.
void execute(char *const argv[]);

#endif
