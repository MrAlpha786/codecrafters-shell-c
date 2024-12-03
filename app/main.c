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

int tokenize(char *);
int sanitizeSingleQuote(int, int);
int sanitizeDoubleQuote(int, int);
void sanitizeWhitespace(char *, int);
int check_flags(char *);
int execute(char *);
BuiltinCommand *find_builtin_command(char *);
char *find_command_in_path(char *);

int repl();
int echo();
int sexit();
int stype();
int spwd();
int scd();
int command_not_found();

BuiltinCommand builtins[] = {
    {name : "echo", runner : echo},
    {name : "exit", runner : sexit},
    {name : "type", runner : stype},
    {name : "pwd", runner : spwd},
    {name : "cd", runner : scd}};

CommandToken TOKENS = {};
unsigned int FLAGS = 0;

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
    if (check_flags(input) < 0)
      continue;

    execute(input);
  }
}

int tokenize(char *input)
{
  char *str = strdup(input);
  int position = 0, last_quote_pos = 0, curr_token = 0, total_tokens = 1;
  char c;

  TOKENS.tokens[0] = str;
  while ((c = str[position]) != '\0')
  {
    if (c == '\'')
    {
      last_quote_pos = sanitizeSingleQuote(curr_token, last_quote_pos);
      position += last_quote_pos;
    }

    if (c == '"')
    {
      last_quote_pos = sanitizeDoubleQuote(curr_token, last_quote_pos);
      position += last_quote_pos;
    }

    if (c == ' ')
    {
      last_quote_pos = -1;
      total_tokens++;
      TOKENS.tokens[++curr_token] = &str[position + 1];
      str[position] = '\0';
      sanitizeWhitespace(str, position + 1);
    }
    position++;
    last_quote_pos++;
  }

  TOKENS.len = total_tokens;
  return 0;
}

int sanitizeSingleQuote(int curr_token, int first_q_pos)
{
  char *s = TOKENS.tokens[curr_token];
  int i = first_q_pos, j = i + 1, next_pos = j;
  char c;
  FLAGS = FLAGS | 1; // set single quote flag

  while ((c = s[i] = s[j]) != '\0' && c != '\'')
  {
    i++;
    j++;
  }

  if (c == '\'')
  {
    FLAGS = FLAGS & ~1; // reset flag
    next_pos = i;
    j++;
  }

  while ((c = s[i] = s[j]) != '\0')
  {
    i++;
    j++;
  }

  return next_pos - first_q_pos - 1;
}

int sanitizeDoubleQuote(int curr_token, int first_q_pos)
{
  char *s = TOKENS.tokens[curr_token];
  int i = first_q_pos, j = i + 1, next_pos = j;
  char c;
  FLAGS = FLAGS | (1 << 1); // set single quote flag

  while ((c = s[i] = s[j]) != '\0' && c != '"')
  {
    i++;
    j++;
  }

  if (c == '"')
  {
    FLAGS = FLAGS & ~(1 << 1); // reset flag
    next_pos = i;
    j++;
  }

  while ((c = s[i] = s[j]) != '\0')
  {
    i++;
    j++;
  }

  return next_pos - first_q_pos - 1;
}

void sanitizeWhitespace(char *s, int i)
{
  int j = i;
  char c;
  while ((c = s[j]) == ' ')
    j++;
  while ((c = s[i] = s[j]) != '\0')
  {
    i++;
    j++;
  }
}

int check_flags(char *input)
{
  if ((FLAGS & 1) == 1 || (FLAGS & (1 << 1)) == (1 << 1))
  {
    printf("syntax error: %s\n", input);
    return -1;
  }
  return 0;
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

int sexit()
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

int scd()
{
  char *path;
  if (strcmp("~", TOKENS.tokens[1]) == 0)
  {
    path = getenv("HOME");
  }
  else
  {
    path = TOKENS.tokens[1];
  }
  int status = chdir(path);
  if (status < 0)
  {
    printf("cd: %s: No such file or directory\n", TOKENS.tokens[1]);
  }
  return 0;
}