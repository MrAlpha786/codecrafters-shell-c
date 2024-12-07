#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "shell.h"
#include "token.h"
#include "builtins.h"

int execute(char *);
int command_not_found();

int repl();

int main()
{
  return repl();
}

int repl()
{
  char input[100];

  while (1)
  {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    fgets(input, 100, stdin);

    // strip the last '\n'
    int l = strlen(input);
    input[l - 1] = '\0';

    tokenize(input);
    // printf("%d\n", TOKENS.len);
    if (check_flags(input) < 0)
      continue;

    execute(input);
  }
}

int command_not_found()
{
  char *command = FIRST_TOKEN.text;
  printf("%s: command not found\n", command);
  return 0;
}

int execute(char *input)
{
  BuiltinCommand *com = find_builtin_command(FIRST_TOKEN.text);
  if (com != NULL)
    return com->runner();

  char *pcom = find_command_in_path(FIRST_TOKEN.text);
  if (pcom != NULL)
  {
    return system(input);
  }

  return command_not_found();
}

char *find_command_in_path(char *command)
{
  char *path = getenv("PATH");
  if (path == NULL)
    return NULL;

  struct stat statbuf;
  static char filepath[100];
  char *path_copy = strdup(path); // do everything on pathcopy because of strtok() uses a shared buffer.
  char *dir = strtok(path_copy, ":");

  while (dir != NULL)
  {
    snprintf(filepath, sizeof(filepath), "%s/%s", dir, command);
    if (stat(filepath, &statbuf) == 0 && statbuf.st_mode & S_IXUSR)
    {
      free(path_copy);
      return filepath;
    }

    dir = strtok(NULL, ":");
  }

  free(path_copy);
  return NULL;
}