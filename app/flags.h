#ifndef FLAGS_H
#define FLAGS_H

#include <stdio.h>

// Enum for defining flags
typedef enum
{
    FLAG_NONE = 0,
    FLAG_SINGLE_QUOTE = 1 << 0,
    FLAG_DOUBLE_QUOTE = 1 << 1,
    FLAG_REDIRRECT_STDIN = 1 << 2,
    FLAG_REDIRRECT_STDERR = 1 << 3
} Flags;

// Function prototypes
void set_flag(int flag);
void clear_flag(int flag);
void toggle_flag(int flag);
int is_flag_set(int flag);
void print_flags();
int get_flags();
void reset_flags();

#endif // FLAGS_H
