#include <stdio.h>
#include <string.h>

int repl();

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

    if (strcmp(input, "exit 0") == 0)
      return 0;

    printf("%s: command not found\n", input);
  }
}
