#include "pipe_n.h"

#ifdef DEBUG
#include <stdio.h>
#endif

static struct pipe_n_entry *pipe_n_table[MAX + 1];
static unsigned entry_idx_max = 0;

int init_pipe_n_table(void)
{
	int i;

	for(i = 0; i <= MAX; ++i)
	{
		pipe_n_table[i] = malloc(sizeof(struct pipe_n_entry));

		pipe_n_table[i]->len = 0;
	}

	return 0;
}

int free_pipe_n_table(void)
{
	int i;

	for(i = 0; i <= MAX; ++i)
		free(pipe_n_table[i]);

	return 0;
}

int add_pipe_n_entry(int n, int p_fd)
{
	pipe_n_table[n]->pipes[pipe_n_table[n]->len++] = p_fd;

	if(n > entry_idx_max)
		entry_idx_max = n;

	return 0;
}

struct pipe_n_entry *get_pipe_n_entry(void)
{
	return pipe_n_table[0];
}

int rotate_pipe_n_table(void)
{
	struct pipe_n_entry *tmp;
	int i;

#ifdef DEBUG
fprintf(stderr, "entry_idx_max = %d\n", entry_idx_max);
#endif
	tmp = pipe_n_table[0];
	for(i = 1; i <= entry_idx_max; ++i)
	{
		pipe_n_table[i - 1] = pipe_n_table[i];
	}

	tmp->len = 0;
	pipe_n_table[entry_idx_max] = tmp;
	entry_idx_max = entry_idx_max > 0 ? entry_idx_max - 1 : 0;

	return 0;
}

unsigned get_pipe_n_max(void)
{
	return entry_idx_max;
}
