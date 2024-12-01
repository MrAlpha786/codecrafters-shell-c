#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int repl();
int command_runner(char *);
int echo(char *, int);
void sexist(char *, int);

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
  int position = 0;
  char c;

  while ((c = input[position]) != ' ' && c != '\0')
    position++;

  input[position] = '\0';
  if (strcmp(input, "echo") == 0)
  {
    input[position] = ' ';
    return echo(input, position);
  }
  else if (strcmp(input, "exit") == 0)
  {
    input[position] = ' ';
    sexist(input, position);
  }

  printf("%s: command not found\n", input);

  return 0;
}

int echo(char *input, int position)
{
  int new_postion = 0;
  char c;

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

void sexist(char *input, int position)
{
  int new_postion = 0;
  char c;

  while ((c = input[position]) == ' ')
    position++;

  while ((c = input[new_postion] = input[position]) != '\0')
  {
    new_postion++;
    position++;
  }

  int code = atoi(input);

  exit(code);
}