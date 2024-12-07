#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell.h"
#include "builtins.h"

static BuiltinCommand builtins[] = {
    {name : "echo", runner : echo},
    {name : "exit", runner : sexit},
    {name : "type", runner : stype},
    {name : "pwd", runner : spwd},
    {name : "cd", runner : scd}};

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

int echo()
{
    CommandToken *curr_token = FIRST_TOKEN.next;
    while (curr_token)
    {
        printf("%s", curr_token->text);
        if (curr_token->next)
        {
            printf(" ");
        }
        curr_token = curr_token->next;
    }
    printf("\n");

    return 0;
}

int sexit()
{
    if (FIRST_TOKEN.next)
    {
        int code = atoi(FIRST_TOKEN.next->text);
        exit(code);
    }
    exit(0);

    return 0;
}

int stype()
{
    CommandToken *curr_token = FIRST_TOKEN.next;
    while (curr_token)
    {
        char *arg = curr_token->text;

        BuiltinCommand *com = find_builtin_command(arg);
        if (com != NULL)
        {
            printf("%s is a shell builtin\n", arg);
        }
        else
        {
            char *filepath = find_command_in_path(arg);
            if (filepath != NULL)
            {
                printf("%s is %s\n", arg, filepath);
            }
            else
            {
                printf("%s: not found\n", arg);
            }
        }

        curr_token = curr_token->next;
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
    char *dir = FIRST_TOKEN.next->text;
    if (strcmp("~", dir) == 0)
    {
        path = getenv("HOME");
    }
    else
    {
        path = dir;
    }
    int status = chdir(path);
    if (status < 0)
    {
        printf("cd: %s: No such file or directory\n", dir);
    }
    return 0;
}