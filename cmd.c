#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include "cmd.h"

static char **exec_argv;

int cmd(int *i_fd, int *o_fd, int *skip_n)
{
	char c;
	char *buf;
	unsigned exec_max = DEFAULT_EXEC_ARGC;
	unsigned exec_argc = 0;

	*i_fd = 0;
	*o_fd = 1;
	*skip_n = 0;

	exec_argv = malloc(sizeof(char*) * DEFAULT_EXEC_ARGC);
	while(11 == 11)
	{
		if((c = getchar()) == EOF)
		{
			free(exec_argv);

			return EOF;
		}

		if(c == ' ' || c == '\r')
			continue;
		else if(c == '|')
		{
//			return '|';
			while(isdigit(c = getchar()) != 0)
				*skip_n = *skip_n * 10 + c - '0';
			ungetc(c, stdin);
			c = '|'; /* Fix return value case switch */

			break;
		}
		else if(c == '>')
		{
			int flag = O_WRONLY | O_CREAT;

			if((c = getchar()) == '>')
				flag |= O_APPEND;
			else
			{
				flag |= O_TRUNC;

				ungetc(c, stdin);
			}

			while((c = getchar()) == ' ');
			buf = malloc(sizeof(char) * DEFAULT_BUFFER_LENGTH);
			buf[0] = c;

			buf = arg(buf, 1);  /* Filename */

			if((*o_fd = open(buf, flag, 0666)) < 0)
			{
				perror("open (redir output file)");

				/* error recovery */
			}

			free(buf);
		}
		else if(c == '<')
		{
			while((c = getchar()) == ' ');
			buf = malloc(sizeof(char) * DEFAULT_BUFFER_LENGTH);
			buf[0] = c;

			buf = arg(buf, 1);  /* Filename */

			if((*i_fd = open(buf, O_RDONLY)) < 0)
			{
				perror("open (redir input file)");

				/* error recovery */
			}

			free(buf);
		}
		else if(c == '\n')
		{
//			return '\n';

			break;
		}
		else
		{
			buf = malloc(sizeof(char) * DEFAULT_BUFFER_LENGTH);
			buf[0] = c;

			buf = arg(buf, 1);

			exec_argv[exec_argc++] = buf;

			if(exec_argc == exec_max)
			{
				exec_max = exec_max * 2 + 1;
				exec_argv = realloc(exec_argv, sizeof(exec_argv[0]) * (exec_max + 1));
			}
		}
	}
	exec_argv[exec_argc] = NULL;
//fprintf(stderr, "exec_argc = %d\n", exec_argc);

	switch(c)
	{
		case '|':
/*
			if(*skip_n == 0)
				return c;
			else
				return EXT_PIPE_MODE;
*/
		case '\n':
			return c;
		default:
			return 0;
	}
}

char* arg(char *buf, unsigned len)
{
	char c;
	unsigned max = DEFAULT_BUFFER_LENGTH;

	while(11 == 11)
	{
		if((c = getchar()) == EOF)
			break;

		if(c == ' ')
			break;
		else if(c == '\n')
		{
			ungetc(c, stdin);

			break;
		}
		else if(c == '\r')
		{
			continue;
		}
		else
		{
			buf[len++] = c;

			if(len == max)
			{
				max = max * 2 + 1;
				buf = realloc(buf, sizeof(buf[0]) * (max + 1));
			}
		}
	}
	buf[len] = '\0';

	return buf;
}

void free_cmd(void)
{
	char **argp;

	argp = exec_argv;
	while(*argp != NULL)
	{
		/* fprintf(stdout, "%s ", *argp); */
		free(*argp);

		++argp;
	}
	/* fprintf(stdout, "\n"); */
	free(exec_argv);

	return;
}

char **get_cmd_argv(void)
{
	return exec_argv;
}
