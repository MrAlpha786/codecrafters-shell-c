#include "shell.h"

#ifndef BUILTINS_H
#define BUILTINS_H

BuiltinCommand *find_builtin_command(char *);
int echo();
int sexit();
int stype();
int spwd();
int scd();

#endif