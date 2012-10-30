#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "cmd.h"
#include <sys/types.h>
#include <sys/wait.h>

struct pipe_i
{
	int rd;
	int wr;
};

void env(void);
int clearenv(void);
void printenv(char *);
int _setenv(char *name, char* value);

int init(char **);

static void sigchld_hdl(int sig)
{
	while(waitpid(-1, NULL, WNOHANG) > 0)
	{
	}

	return;
}

int main(int argc, char **argv)
{
	char **exec_argv;
	pid_t pid;
	int mode;
	int cmd_in, cmd_out;
	struct pipe_i p;
	int std_in, std_out, std_err;

	init(NULL);
	while(11 == 11)
	{
		prev_mode = -1;
		fprintf(stdout, "%% ");

		while(11 == 11)
		{
			if((mode = cmd(&in, &out)) == EOF)
				goto EXIT;

/* fprintf(stdout, "[DEBUG] mode = %d \n", mode); */
			exec_argv = get_cmd_argv();
			/* fprintf(stdout, "%s\n", exec_argv[0]); */
/*
char **argp = exec_argv;
while(*argp != NULL)
{
	fprintf(stderr, "%s\n", *argp);
	++argp;
}
*/

			/* Shell command */
			if(strcmp(exec_argv[0], "printenv") == 0)
			{
				printenv(exec_argv[1]);

				free_cmd();
				break;
			}
			else if(strcmp(exec_argv[0], "setenv") == 0)
			{
				if(exec_argv[2] == NULL)
					_setenv(exec_argv[1], "");
				else
					_setenv(exec_argv[1], exec_argv[2]);

				free_cmd();
				break;
			}
			else  /* Normal command */
			{
				if(prev_mode == '|')
					in = p.rd;

				if(mode == '|')
				{
					if(pipe((int *)&p) == -1)
						perror("pipe");

					out = p.wr;
				}

/*
fprintf(stderr, "[DEBUG] prev_mode = %c, mode = %c\n", prev_mode, mode);
fprintf(stderr, "[DEBUG] p.rd = %d, p.wr = %d\n", p.rd, p.wr);
fprintf(stderr, "[DEBUG] in = %d, out = %d\n", in, out);
*/
				if((pid = fork()) == 0)  /* Child process */
				{
/*
fprintf(stderr, "[CHILD] forked\n");
*/
					if(mode == '|')  /* ...  */
						close(p.rd);

					if(dup2(in, STDIN_FILENO) == -1)
						perror("dup2");
					if(in != STDIN_FILENO)
						close(in);

					if(dup2(out, STDOUT_FILENO) == -1)
						perror("dup2");
					if(out != STDOUT_FILENO)
						close(out);

					if(execvp(exec_argv[0], exec_argv) == -1)
					{
						perror("execvp");

						exit(EXIT_FAILURE);
					}
				}
				else
				{
/*
fprintf(stderr, "[PARENT] forked\n");
*/
					if(mode == '|')
						close(p.wr);

					if(prev_mode == '|')
						close(in);

					if(mode == '\n')
					{
						waitpid(pid, NULL, 0);
						while(waitpid(-1, NULL, WNOHANG) > 0)
						{
						}

						break;
					}
				}
			}

			free_cmd();
			prev_mode = mode;
		}
	}
EXIT:
	fprintf(stdout, "\n");

	return 0;
}

void env(void)
{
	extern char **environ;
	char **envp = environ;

	while(*envp != NULL)
		fprintf(stdout, "%s\n", *envp++);
	
	return;
}

int clearenv(void)
{
	extern char **environ;
	char **envp = environ;

	char buf[97], *ptr;
	int len;

	while(*envp != NULL)
	{
		ptr = strchr(*envp, '=');
		len = ptr - *envp;

		memcpy(buf, *envp, len);
		buf[len] = '\0';

/* fprintf(stdout, "%s\n", buf); */
		if(unsetenv(buf) == -1)
			return -1;

		++envp;
	}

	return 0;
}

void printenv(char *var)
{
	char *ptr;

	if(var == NULL)
		env();
	else
	{
		if((ptr = getenv(var)) != NULL)
			fprintf(stdout, "%s\n", ptr);
	}
	
	return;
}

int _setenv(char *name, char *value)
{
	return setenv(name, value, 1);
}

int init(char **envi)
{
	if(clearenv() == -1)
		return -1;
	
	if(envi != NULL)
	{
		char **envp = envi;

		while(envp != NULL)
			if(putenv(*envp++) == -1)
				return -1;
	}
	else
	{
		setenv("PATH", "bin:.", 0);
	}

	return 0;
}
