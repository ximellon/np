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
	struct pipe_i pipe_out, pipe_err;
	int std_in, std_out, std_err;  /* */

	init(NULL);
	std_in  = STDIN_FILENO;
	std_out = STDOUT_FILENO;
	std_err = STDERR_FILENO;

	while(11 == 11)
	{
		fprintf(stdout, "%% ");

		while(11 == 11)
		{
			if((mode = cmd(&cmd_in, &cmd_out)) == EOF)
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
				if(mode == '|')
				{
					if(pipe((int *)&pipe_out) == -1)
						perror("pipe");

					std_out = pipe_out.wr;
					/* check cmd_out */
				}

//fprintf(stderr, "[DEBUG] mode = %c\n", mode);
fprintf(stderr, "[DEBUG] pipe_out.rd = %d, pipe_out.wr = %d\n", pipe_out.rd, pipe_out.wr);
//fprintf(stderr, "[DEBUG] std_in = %d, std_out = %d, std_err = %d\n", std_in, std_out, std_err);
				if((pid = fork()) == 0)  /* Child process */
				{
/*
fprintf(stderr, "[CHILD] forked\n");
*/
					if(mode == '|')  /* ...  */
						close(pipe_out.rd);

					if(std_in != STDIN_FILENO)
					{
						if(dup2(std_in, STDIN_FILENO) == -1)
							perror("dup2 (std_in)");

						close(std_in);
					}

					if(std_out != STDOUT_FILENO)
					{
						if(dup2(std_out, STDOUT_FILENO) == -1)
							perror("dup2 (std_out)");

						close(std_out);
					}

					if(std_err != STDERR_FILENO)
					{
						if(dup2(std_err, STDERR_FILENO) == -1)
							perror("dup2 (std_err)");

						close(std_err);
					}

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
						close(pipe_out.wr);

					if(std_in != STDIN_FILENO)
						close(std_in);

					/************* FOR NEXT EXECUTION *************/

					std_in  = STDIN_FILENO;
					std_out = STDOUT_FILENO;
					std_err = STDERR_FILENO;

					if(mode == '|')  /* Prepare the fd of stdin for the next cmd */
						std_in = pipe_out.rd;

					if(mode == '\n')  /* END */
					{
						waitpid(pid, NULL, 0);
						while(waitpid(-1, NULL, WNOHANG) > 0)
						{
						}

						free_cmd();
						break;
					}
				}
			}

			free_cmd();
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
