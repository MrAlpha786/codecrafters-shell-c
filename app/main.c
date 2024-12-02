#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct builtin_command_t
{
  char *name;
  int (*runner)();
} BuiltinCommand;

typedef struct command_token_t
{
  int len;
  char *tokens[100];
} CommandToken;

void tokenize(char *);
int execute(char *);
BuiltinCommand *find_builtin_command(char *);
char *find_command_in_path(char *);

int repl();
int echo();
int sexist();
int stype();
int spwd();
int command_not_found();

BuiltinCommand builtins[] = {
    {name : "echo", runner : echo},
    {name : "exit", runner : sexist},
    {name : "type", runner : stype},
    {name : "pwd", runner : spwd}};

CommandToken TOKENS = {};

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
    execute(input);
  }
}

void tokenize(char *input)
{
  char *str = strdup(input);
  int position = 0, total_tokens = 1;
  char c;
  while ((c = str[position]) != '\0')
  {
    if (c == ' ')
      total_tokens++;
    position++;
  }

  TOKENS.len = total_tokens;
  TOKENS.tokens[0] = str;
  int current_token = 1;
  position = 0;
  while ((c = str[position]) != '\0')
  {
    if (c == ' ')
    {
      TOKENS.tokens[current_token++] = &str[++position];
      str[position - 1] = '\0';
    }
    position++;
  }
}

int execute(char *input)
{
  BuiltinCommand *com = find_builtin_command(TOKENS.tokens[0]);
  if (com != NULL)
    return com->runner();

  char *pcom = find_command_in_path(TOKENS.tokens[0]);
  if (pcom != NULL)
  {
    return system(input);
  }

  return command_not_found();
}

BuiltinCommand *find_builtin_command(char *command)
{
  for (int i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
  {
    if (strcmp(builtins[i].name, command) == 0)
    {
      return &builtins[i];
    }
  }

  return NULL;
}

char *find_command_in_path(char *command)
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

int command_not_found()
{
  char *command = TOKENS.tokens[0];
  printf("%s: command not found\n", command);
  return 0;
}

int echo()
{
  for (int i = 1; i < TOKENS.len; i++)
  {
    printf("%s", TOKENS.tokens[i]);
    if (i < TOKENS.len - 1)
      printf(" ");
  }
  printf("\n");

  return 0;
}

int sexist()
{
  if (TOKENS.len > 1)
  {
    int code = atoi(TOKENS.tokens[1]);
    exit(code);
  }
  exit(0);

  return 0;
}

int stype()
{
  for (int i = 1; i < TOKENS.len; i++)
  {
    char *arg = TOKENS.tokens[i];

    BuiltinCommand *com = find_builtin_command(arg);
    if (com != NULL)
    {
      printf("%s is a shell builtin\n", arg);
      continue;
    }

    char *filepath = find_command_in_path(arg);
    if (filepath != NULL)
    {
      printf("%s is %s\n", arg, filepath);
      free(filepath);
    }
    else
    {
      printf("%s: not found\n", arg);
    }
  }

  return 0;
}

int spwd()
{
  char *pwd = getcwd(NULL, 0);
  printf("%s\n", pwd);
  free(pwd);
  return 0;
}