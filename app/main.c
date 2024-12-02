#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
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

char *is_command_in_path(char *command, char *filepath)
{
  char *paths = getenv("PATH");
  if (paths == NULL)
    return NULL;

  DIR *d;
  struct dirent *dir;
  struct stat statbuf;
  char path[100], c;
  int i = 0, j = 0;

  while (paths[j] != '\0')
  {
    while ((c = path[i] = paths[j]) != '\0')
    {
      if (c == ':')
      {
        path[i] = '\0';
        i = 0;
        j++;
        break;
      }
      i++;
      j++;
    }

    d = opendir(path);
    if (d == NULL)
      continue;

    while ((dir = readdir(d)) != NULL)
    {
      if (strcmp(dir->d_name, command) != 0)
        continue;

      closedir(d);
      filepath[0] = '\0';
      strcat(filepath, path);
      strcat(filepath, "/");
      strcat(filepath, dir->d_name);
      if (stat(dir->d_name, &statbuf) == 0 && statbuf.st_mode & S_IXUSR)
      {
        return filepath;
      }
      return NULL;
    }
  }

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
  int position = 4, new_postion = 0;
  char c, filepath[100];

  // Remove whitespace before the string arg
  while ((c = input[position]) == ' ')
    position++;

  while ((c = input[new_postion] = input[position]) != '\0')
  {
    new_postion++;
    position++;
  }

  BuiltinCommand *com = find_command(input);

  if (com != NULL)
  {
    printf("%s is a shell builtin\n", input);
    return 0;
  }

  filepath[0] = '\0';
  is_command_in_path(input, filepath);
  if (filepath[0] != '\0')
  {
    printf("%s is %s\n", input, filepath);
    return 0;
  }

  printf("%s: not found\n", input);

  return 0;
}