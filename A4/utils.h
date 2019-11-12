#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

#define DEPOT_NAME_ARGS 1
#define CARRIAGE_RETURN_ASCII 13
#define NEWLINE_ASCII 10
#define SPACE_ASCII 32
#define COLON_ASCII 58
#define COMMAND_THRESHOLD 2
#define BUFFER_DEFAULT 100
#define MIN_ARGS 2

int contains_banned_char(char*);
int check_args(int, char**);
int compare_resource_names(const void*, const void*);
int compare_neighbour_names(const void*, const void*);
char** string_split(char*, char, int*);
int get_key(char*);
char* get_command(char*);
char* read_fd_line(int);
int amount_of_colons(char*);
char* read_line(FILE*);

#endif