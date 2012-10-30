#CFLAGS += -ansi

shell: shell.o cmd.o pipe_n.o
	$(CC) $^ -o $@
