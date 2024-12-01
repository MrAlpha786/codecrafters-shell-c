#include <stdio.h>
#include <string.h>

int main()
{

  while (1 == 1)
  {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    // strip the last '\n'
    input[strlen(input) - 1] = '\0';

    printf("%s: command not found\n", input);
  }

  return 0;
}
