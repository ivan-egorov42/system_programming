#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define verify(cmd) \
	if((cmd) < 0) \
	{ \
		perror(#cmd); \
		exit(errno); \
	}

const int max_args = 100;
const int max_len  = 100;
const int max_str  = 100;
char** parser(char* line, int* num, const char* symbols);

int main(int argc, char* argv[])
{
	char* str;
	size_t len = 0;
	while(1)
	{
        printf("prompt$ ");
		getline(&str, &len, stdin);
		
		int cmd_num = 0;
		char** commands = parser(str, &cmd_num, "|");
		
		int next_fd = 0;
		for(int i = 0; i < cmd_num; ++i)
		{	
			int arg_num = 0;
			char** arguments = parser(commands[i], &arg_num, " \n\t\0");
			
			int fd[2] = {};
			verify(pipe(fd));
			int read_fd = fd[0];
			int write_fd = fd[1];
			
			pid_t pid = fork();
			verify(pid);
			
			if(pid == 0)
			{
				if(i != 0) 
				{
					verify(close(0));
					verify(dup(next_fd));
					close(next_fd);
				}
				verify(close(read_fd));
				
				if(i != cmd_num - 1)
				{
					verify(close(1));
					verify(dup(write_fd));
				}
				verify(close(write_fd));
				
				verify(execvp(arguments[0], arguments));
			}
			
			wait(NULL);
			free(arguments);
			next_fd = read_fd;
			verify(close(write_fd));
		}
		free(commands);
	}
	free(str);
	return 0;
}

char** parser(char* line, int* num, const char* symbols)
{
	char** str = (char**) calloc(max_args, sizeof(char*));
	int counter = 0;
	char* word = strtok(line, symbols);
	while(word != NULL)
	{
		str[counter] = (char*) calloc(max_len, sizeof(char));
		strcpy(str[counter++], word);
		word = strtok(NULL, symbols);
	}
	if((strcmp(str[0], "quit") == 0) || (strcmp(str[0], "exit") == 0))
		exit(0);

	*num = counter;
	return str;
}