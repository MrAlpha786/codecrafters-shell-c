#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct builtin_command_t
{
  char *name;
  char *desc;
  int (*runner)(char *);
} BuiltinCommand;

int repl();
int echo(char *);
int sexist(char *);
int stype(char *);
int command_not_found(char *);
BuiltinCommand find_command(char *);

BuiltinCommand builtins[] = {
    {name : "echo", desc : "echo is a shell builtin", runner : echo},
    {name : "exit", desc : "exit is a shell builtin", runner : sexist},
    {name : "type", desc : "type is a shell builtin", runner : stype}};

BuiltinCommand non_existing_command = {
  name : "",
  desc : "not found",
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

    BuiltinCommand com = find_command(input);

    com.runner(input);
  }
}

BuiltinCommand find_command(char *command)
{
  int len = 0;
  char c;
  while ((c = command[len]) != '\0' && c != ' ')
    len++;

  for (int i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
  {
    if (strncmp(builtins[i].name, command, len) == 0)
    {
      return builtins[i];
    }
  }

  return non_existing_command;
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
  char c;

  // Remove whitespace before the string arg
  while ((c = input[position]) == ' ')
    position++;

  while ((c = input[new_postion] = input[position]) != '\0')
  {
    new_postion++;
    position++;
  }

  BuiltinCommand com = find_command(input);

  if (com.name == "")
  {
    printf("%s: not found\n", input);
  }
  else
  {
    printf("%s\n", com.desc);
  }

  return 0;
}