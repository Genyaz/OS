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

char** split_string(char * s, char * delimiters)
{
	char ** res = NULL;
	char * p = (char*)strtok(s, delimiters);
	//write_(STDOUT_FILENO, "Splitting \"", 11);
	//write_(STDOUT_FILENO, s, strlen(s));
	//write_(STDOUT_FILENO, "\"\n", 2);	
	int size = 0, i;
	while (p) 
	{
		res = realloc(res, sizeof(char*) * ++size);
		if (res == NULL) exit(EXIT_FAILURE);
		res[size - 1] = p;
		//write_(STDOUT_FILENO, p, strlen(p));
		//write_(STDOUT_FILENO, "\n", 2);		
 		p = (char*)strtok(NULL, delimiters);
	}
	res = realloc(res, sizeof(char*) * (size + 1));
	res[size] = 0;
	return res;
}

struct execargs_t * ea_from_string(char* s)
{
	//write_(STDOUT_FILENO, "Program \"", 9);
	//write_(STDOUT_FILENO, s, strlen(s));
	//write_(STDOUT_FILENO, "\"\n", 2);
	char ** split = split_string(s, " ");
	if (split[0] == NULL) return NULL;
	//write_(STDOUT_FILENO, "Main \"", 6);
	//write_(STDOUT_FILENO, split[0], strlen(split[0]));
	//write_(STDOUT_FILENO, "\"\n", 2);
	int i = 1;
	while (split[i] != NULL)
	{
		//write_(STDOUT_FILENO, "Arg \"", 5);
		//write_(STDOUT_FILENO, split[i], strlen(split[i]));
		//write_(STDOUT_FILENO, "\"\n", 2);
		i++;
	}
	return execargs_new(split[0], split);
}

void run(char * ss, int len)
{
	//write_(STDOUT_FILENO, "Parsing \"", 9);
	//write_(STDOUT_FILENO, ss, len);
	//write_(STDOUT_FILENO, "\"\n", 2);
	char * s = malloc(sizeof(char) * (len + 1));
	strncpy(s, ss, len + 1);
	s[len] = 0;
	char ** split = split_string(s, "|");
	int n = 0, i, size = 0;	
	while (split[n] != NULL) n++;
	execargs_t ** programs = malloc(sizeof(execargs_t*) * (n + 1));
	if (programs == NULL) exit(EXIT_FAILURE);
	for (i = 0; i < n; i++) 
	{
		programs[size++] = ea_from_string(split[i]);
		if (programs[size - 1] == NULL) size--;
	}
	programs[size] = NULL;
	if (size > 0)
	{
		//write_(STDOUT_FILENO, "Running pipeline... \n", 21);
		runpiped(programs, size);
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
			while (write_(STDOUT_FILENO, "$", 1) == -1)
			{
				if (errno != EINTR)
				{
					exit(EXIT_FAILURE);
				}
			}				
			run_before = 0;
		}
		int r = read(STDIN_FILENO, buf + buf_size, BUF_SIZE - buf_size);
		if (r == 0) 
		{
			last_run();
			exit(EXIT_SUCCESS);
		}	
		if (r == -1 && errno != EINTR) exit(EXIT_FAILURE);
		if (r > 0)
		{
			buf_size += r;
			check_for_run();
		}	
	}
	exit(EXIT_SUCCESS);
}
