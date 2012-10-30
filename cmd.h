#ifndef __CMD_H__
#define __CMD_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEFAULT_BUFFER_LENGTH 127
#define DEFAULT_EXEC_ARGC 31

/*
#define CMDIO_STDIO 0
#define CMDIO_PIPE '|'
#define CMDIO_REDIR '>'
*/

int cmd(int *, int *);
char* arg(char *, unsigned);
void free_cmd(void);
char** get_cmd_argv(void);
#endif
