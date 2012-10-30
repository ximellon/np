#ifndef __PIPE_N_H__
#define __PIPE_N_H__

#include <stdlib.h>

#define MAX 1000

struct pipe_n_entry
{
	unsigned len;
	int pipes[MAX];
};

int init_pipe_n_table(void);
int free_pipe_n_table(void);
int add_pipe_n_entry(int, int);
struct pipe_n_entry *get_pipe_n_entry(void);
int rotate_pipe_n_table(void);
unsigned get_pipe_n_max(void);
#endif
