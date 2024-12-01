#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int repl();
int command_runner(char *);
int echo(char *);
void sexist(char *);

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

    command_runner(input);
  }
}

int command_runner(char *input)
{
  if (strncmp(input, "echo", 4) == 0)
  {
    if (input[4] == ' ' || input[4] == '\0')
      return echo(input);
  }
  else if (strncmp(input, "exit", 4) == 0)
  {
    sexist(input);
  }

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

void sexist(char *input)
{
  int code = atoi(input);

  exit(code);
}