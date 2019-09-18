#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>   
#include <unistd.h>

/**
*   Reads the line from a given file descriptor until it 
*		reaches a new line and returns the string that was found
*/
char* read_fd_line(int fd) {
    char* result = malloc(sizeof(char) * 80);
    int position = 0;
    char buff[1];
    while (1) { 
        read(fd, buff, 1);
        if (buff[0] == EOF || buff[0] == '\n' || buff[0] == 0) {
            result[position] = '\0';
            return result;
        } else {
            result[position++] = buff[0];
        }
    }
}

