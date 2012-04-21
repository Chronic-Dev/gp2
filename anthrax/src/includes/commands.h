#ifndef ACCD_COMMANDS
#define ACCD_COMMANDS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <dirent.h>
#include <dlfcn.h>

int cmd_put(int wfd, int argc, char **args);
int cmd_get(int wfd, int argc, char **args);
int cmd_mod(int wfd, int argc, char **args);

typedef int (*isUsableCommand_t)(char *);
typedef int (*processCommand_t)(int, char *, int, char **);

int gModc;
void **gMods;

#endif
