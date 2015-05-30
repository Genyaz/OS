#define BUF_SIZE 4096
#define _POSIX_SOURCE
#include <helpers.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

char buf[BUF_SIZE];
int buf_size = 0;
int run_before = 1;

void hdl(int sig)
{
}

char** split_string(char * s, char * delimiters)
{
	char ** res = NULL;
	char * p = (char*)strtok(s, delimiters);
	int size = 0;
	while (p)
	{
		res = realloc(res, sizeof(char*) * ++size);
		if (res == NULL) exit(EXIT_FAILURE);
		res[size - 1] = p;
 		p = (char*)strtok(NULL, delimiters);
	}
	res = realloc(res, sizeof(char*) * (size + 1));
	res[size] = NULL;
	return res;
}

struct execargs_t * ea_from_string(char* s)
{
	char ** split = split_string(s, " ");
	if (split[0] == NULL) return NULL;
	return execargs_new(split[0], split);
}

void run(char * ss, int len)
{
	char * s = malloc(sizeof(char) * (len + 1));
	if (s == NULL) exit(EXIT_FAILURE);
	strncpy(s, ss, len + 1);
	s[len] = 0;
	char ** split = split_string(s, "|");
	execargs_t ** programs = NULL;
	int i = 0, size = 0;
	while (split[i] != NULL)
	{
		execargs_t * program = ea_from_string(split[i]);
		if (program != NULL)
		{
			programs = realloc(programs, sizeof(execargs_t*) * ++size);
			if (programs == NULL) exit(EXIT_FAILURE);
			programs[size - 1] = program;
		}
		i++;	
	}
	programs = realloc(programs, sizeof(execargs_t*) * (size + 1));
	if (programs == NULL) exit(EXIT_FAILURE);
	programs[size] = NULL;
	if (size > 0)
	{
		if (runpiped(programs, size) == -1)
		{
			if (write_(STDOUT_FILENO, "\nSomething went wrong, try again.\n", 34) == -1) exit(EXIT_FAILURE); 
		}
		free(s);
		free(split);
		while (--size >= 0)
		{
			free(programs[size]->args);
			free(programs[size]);
		}
		free(programs);		
		run_before = 1;
	}
}

void check_for_run()
{
	int i;
	int begin = 0;
	for (i = 0; i < buf_size; i++)
	{
		if (buf[i] == '\n')
		{
			if (i != begin)
			{
				run(buf + begin, i - begin);
			}
			begin = i + 1;
		}
	}
	memcpy(buf, buf + begin, buf_size - begin);
	buf_size -= begin;
}

void last_run()
{
	buf[buf_size] = '\n';
	buf_size++;
	check_for_run();
}

int main()
{
	struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = hdl;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    sigaction(SIGINT, &act, 0);
	while (1)
	{
		if (run_before)
		{
			if (write_(STDOUT_FILENO, "$", 1) == -1) exit(EXIT_FAILURE);
			run_before = 0;
		}
		int r = read(STDIN_FILENO, buf + buf_size, BUF_SIZE - buf_size);
		if (r == 0) 
		{
			last_run();
			exit(EXIT_SUCCESS);
		}	
		if (r == -1) 
		{
			if (errno != EINTR) exit(EXIT_FAILURE);
			if (write_(STDOUT_FILENO, "\n$", 2) == -1) exit(EXIT_FAILURE);				
			run_before = 0;
		}
		if (r > 0)
		{
			buf_size += r;
			check_for_run();
		}	
	}
}
