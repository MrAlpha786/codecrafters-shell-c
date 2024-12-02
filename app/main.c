#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct builtin_command_t
{
  char *name;
  int (*runner)(char *);
} BuiltinCommand;

int repl();
int echo(char *);
int sexist(char *);
int stype(char *);
int command_not_found(char *);
BuiltinCommand *find_command(char *);

BuiltinCommand builtins[] = {
    {name : "echo", runner : echo},
    {name : "exit", runner : sexist},
    {name : "type", runner : stype}};

BuiltinCommand non_existing_command = {
  name : "",
  runner : command_not_found
};

int main()
{
  return repl();
}

int repl()
{
  while (1)
  {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    // strip the last '\n'
    int l = strlen(input);
    input[l - 1] = '\0';

    BuiltinCommand *com = find_command(input);

    if (com == NULL)
      command_not_found(input);
    else
      com->runner(input);
  }
}

BuiltinCommand *find_command(char *command)
{
  int len = 0;
  char c;
  while ((c = command[len]) != '\0' && c != ' ')
    len++;

  for (int i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
  {
    if (strncmp(builtins[i].name, command, len) == 0)
    {
      return &builtins[i];
    }
  }

  return NULL;
}

char *is_command_in_path(char *command)
{
  char *path = getenv("PATH");
  if (path == NULL)
    return NULL;

  struct stat statbuf;
  static char filepath[100];
  char *path_copy = strdup(path);
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

int command_not_found(char *input)
{
  printf("%s: command not found\n", input);
  return 0;
}

int echo(char *input)
{
  // first five characters are "echo "
  int position = 4, new_postion = 0;
  char c;

  // Remove whitespace before the string arg
  while ((c = input[position]) == ' ')
    position++;

  while ((c = input[new_postion] = input[position]) != '\0')
  {
    new_postion++;
    position++;
  }

  printf("%s\n", input);

  return 0;
}

int sexist(char *input)
{
  int code = atoi(input);
  exit(code);

  return 0;
}

int stype(char *input)
{
  // first five characters are "echo "
  char *input_copy = strdup(input);
  int position = 4, new_postion = 0;
  char c;

  // Remove whitespace before the string arg
  while ((c = input_copy[position]) == ' ')
    position++;

  while ((c = input_copy[new_postion] = input_copy[position]) != '\0')
  {
    new_postion++;
    position++;
  }

  BuiltinCommand *com = find_command(input_copy);
  if (com != NULL)
  {
    printf("%s is a shell builtin\n", input_copy);
    return 0;
  }

  char *filepath = is_command_in_path(input_copy);
  if (filepath != NULL)
  {
    printf("%s is %s\n", input_copy, filepath);
  }
  else
  {
    printf("%s: not found\n", input_copy);
  }
  
  return 0;
}