#include <stdio.h>

#include "flags.h"

unsigned int flags = FLAG_NONE;

// Function to set a flag
void set_flag(int flag)
{
    flags |= flag;
}

// Function to clear a flag
void clear_flag(int flag)
{
    flags &= ~flag;
}

// Function to toggle a flag
void toggle_flag(int flag)
{
    flags ^= flag;
}

// Function to check if a flag is set
int is_flag_set(int flag)
{
    return (flags & flag) != 0;
}

// Function to print the current flag status
void print_flags(int flags)
{
    printf("Current Flags: %d\n", flags);
}

int get_flags()
{
    return flags;
}

void reset_flags()
{
    flags = FLAG_NONE;
}