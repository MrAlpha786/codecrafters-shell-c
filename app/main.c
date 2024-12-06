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
int handleSingleQuote(char *, int);
int handleDoubleQuote(char *, int);
int handleWhitespace(char *, int);
void handleEscapeChar(char *, int);
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
    // printf("%d\n", TOKENS.len);
    if (check_flags(input) < 0)
      continue;

    execute(input);
  }
}

int tokenize(char *input)
{
  char *str = strdup(input);
  int position = 0, curr_token = 0;
  char c;

  TOKENS.tokens[0] = str;
  TOKENS.len = 1;
  while ((c = str[position]) != '\0')
  {
    switch (c)
    {
    case '\\':
      handleEscapeChar(str, position);
      position++;
      break;

    case '\'':
      position = handleSingleQuote(str, position);
      break;

    case '"':
      position = handleDoubleQuote(str, position);
      break;

    case ' ': // current token ends here.
      str[position] = '\0';

      // skip multiple spaces before starting a new token
      position = handleWhitespace(str, position + 1);
      if (str[position] == '\0') // if input ended then break
        break;

      // create a new token
      TOKENS.tokens[++curr_token] = &str[position];
      TOKENS.len++;
      break;

    default:
      position++; // keep adding new char to token
      break;
    }
  }

  return 0;
}

int handleSingleQuote(char *s, int sq_pos)
{
  int i = sq_pos, j = i + 1;
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
    s[j++] = ' ';
  }

  while ((c = s[i] = s[j]) != ' ' && c != '\0')
  {
    i++;
    j++;
  }

  return i;
}

int handleDoubleQuote(char *s, int dq_pos)
{
  int i = dq_pos, j = i + 1, new_pos;
  char c;
  FLAGS = FLAGS | (1 << 1); // set single quote flag

  while ((c = s[i] = s[j]) != '\0' && c != '"')
  {
    if (c == '\\' && (s[j + 1] == '\\' || s[j + 1] == '$' || s[j + 1] == '"'))
    {
      handleEscapeChar(s, j);
      s[i] = s[j]; // recopy s[i] to s[j] because s[j] has changed after handleEscapeChar
    }
    i++;
    j++;
  }

  if (c == '"')
  {
    FLAGS = FLAGS & ~(1 << 1); // reset flag
    s[j++] = ' ';
  }

  while ((c = s[i] = s[j]) != '\0' && c != ' ')
  {
    if (c == '\\')
    {
      handleEscapeChar(s, j);
      s[i] = s[j];
    }
    i++;
    j++;
  }

  return i;
}

int handleWhitespace(char *s, int spc_pos)
{
  int j = spc_pos;
  char c;
  while ((c = s[j]) == ' ')
    j++;
  return j;
}

void handleEscapeChar(char *s, int esc_pos)
{
  int i = esc_pos, j = i + 1;
  char c;

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
    // for (int i = 0; i < TOKENS.len; i++)
    // {
    //   printf("%s\n", TOKENS.tokens[i]);
    // }
    printf("syntax error (%d): %s\n", FLAGS, input);
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