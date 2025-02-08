#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "flags.h"

CommandToken FIRST_TOKEN = {NULL, NULL};

static int handleSingleQuote(char *s, int);
static int handleDoubleQuote(char *s, int);
static int handleWhitespace(char *s, int);
static void handleEscapeChar(char *s, int);
static void free_tokens();

int tokenize(char *input)
{
  free_tokens(); // free the memory, if there were previous tokens
  char *str = strdup(input);
  int position = 0;
  char c;
  CommandToken *curr_token = &FIRST_TOKEN;

  curr_token->text = str;
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
      CommandToken *new_token = malloc(sizeof(CommandToken));
      if (!new_token)
      {
        perror("Failed to allocate memory");
        exit(1);
      }
      new_token->text = &str[position];
      new_token->next = NULL;
      curr_token->next = new_token;
      curr_token = curr_token->next;
      break;

    default:
      position++; // keep adding new char to token
      break;
    }
  }

  return 0;
}

static void free_tokens()
{
  CommandToken *curr = FIRST_TOKEN.next;
  free(FIRST_TOKEN.text); // only free this one, because all others are pointed to its indexes.
  while (curr)
  {
    CommandToken *temp = curr;
    curr = curr->next;
    free(temp); // Free the token
  }
  FIRST_TOKEN.next = NULL; // Reset
}

static int handleSingleQuote(char *s, int sq_pos)
{
  int i = sq_pos, j = i + 1, new_pos = i;
  char c;
  set_flag(FLAG_SINGLE_QUOTE);

  while ((c = s[i] = s[j]) != '\'' && c != '\0')
  {
    i++;
    j++;
  }

  if (c == '\'')
  {
    clear_flag(FLAG_SINGLE_QUOTE);
    j++;
  }

  new_pos = i;

  while ((c = s[i] = s[j]) != '\0')
  {
    i++;
    j++;
  }

  return new_pos;
}

static int handleDoubleQuote(char *s, int dq_pos)
{
  int i = dq_pos, j = i + 1, new_pos = i;
  char c;
  set_flag(FLAG_DOUBLE_QUOTE);

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
    clear_flag(FLAG_DOUBLE_QUOTE);
    j++;
  }

  new_pos = i;

  while ((c = s[i] = s[j]) != '\0')
  {
    i++;
    j++;
  }

  return new_pos;
}

static int handleWhitespace(char *s, int spc_pos)
{
  int j = spc_pos;
  char c;
  while ((c = s[j]) == ' ')
    j++;
  return j;
}

static void handleEscapeChar(char *s, int esc_pos)
{
  int i = esc_pos, j = i + 1;
  char c;

  while ((c = s[i] = s[j]) != '\0')
  {
    i++;
    j++;
  }
}