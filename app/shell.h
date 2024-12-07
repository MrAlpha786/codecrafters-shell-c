#ifndef SHELL
#define SHELL

typedef struct CommandToken
{
    char *text;
    struct CommandToken *next;
} CommandToken;

extern CommandToken FIRST_TOKEN;

typedef struct builtin_command_t
{
    char *name;
    int (*runner)();
} BuiltinCommand;

char *find_command_in_path(char *);

#endif
