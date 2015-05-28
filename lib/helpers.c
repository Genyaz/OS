#define _POSIX_SOURCE
#include "helpers.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef struct execargs_t execargs_t;

ssize_t read_(int fd, void* buf, size_t count) 
{
    size_t already_read = 0;
    ssize_t read_now = 0;
    while (already_read < count)
    {
        if ((read_now = read(fd, buf + already_read, count - already_read)) == -1) 
        {
            return -1;
        }
        else if (read_now == 0)
        {
            return already_read;
        }
        already_read += read_now;
    }
    return count;
}

ssize_t write_(int fd, void* buf, size_t count)
{
    size_t written = 0;
    ssize_t written_now = 0;
    while (written < count)
    {        
        if ((written_now = write(fd, buf + written, count - written)) == -1 && errno != EINTR)
        {
            return -1;
        }
        written += written_now;
    }
    return count;
}

ssize_t read_until(int fd, void * buf, size_t count, char delimiter)
{
    size_t already_read = 0;
    ssize_t read_now = 0;
    char* ch_buf = (char*)buf;
    while (already_read < count)
    {
        if ((read_now = read(fd, buf + already_read, count - already_read)) == -1)
        {
            return -1;
        }
        else if (read_now == 0)
        {
            return already_read;
        }
        else
        {
            int delimiter_in_read = 0;
            for (; read_now > 0; read_now--)
            {
                delimiter_in_read |= ch_buf[already_read] == delimiter;
                already_read++;
            }
            if (delimiter_in_read)
            {
                return already_read;
            }
        }
    }
    return count;
}

int was_interrupted(int return_value)
{
    if (return_value == -1 && errno != EINTR)
    {
        exit(EXIT_FAILURE);
    }
    return (return_value == -1);
}

int spawn(const char* file, char * const argv[])
{
    int pid;
    if ((pid = fork()) == -1)
    {
        exit(EXIT_FAILURE);
    }
    if (pid != 0)
    {
        int status;
        while (was_interrupted(wait(&status)));
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status))
        {
            return -WTERMSIG(status);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else
    {   
        int dev_null = open("/dev/null", O_WRONLY);
        if (dev_null == -1)
        {
            exit(EXIT_FAILURE);
        }
        while (was_interrupted(dup2(dev_null, STDOUT_FILENO)));
        while (was_interrupted(dup2(dev_null, STDERR_FILENO)));
        while (was_interrupted(close(dev_null)));
        execvp(file, argv);
        exit(EXIT_FAILURE);
    }      
}

execargs_t * execargs_new(char* file, char** args)
{
	execargs_t* res = malloc(sizeof(execargs_t));
	if (res == NULL) return NULL;
	res->file = file;
    res->args = args;
	return res;
}

int exec(execargs_t * args)
{
    execvp(args->file, args->args);
    exit(EXIT_FAILURE);
}

pid_t * pids = 0;
size_t pidn = 0;

void kill_chlds()
{
	int i;
	for (i = 0; i < pidn; i++) if (pids[i] != 0)
	{
		kill(pids[i], SIGKILL);
	}
	pidn = 0;
}

void wait_all()
{
	int status;
	while (1)
	{
		wait(&status);
		if (errno == ECHILD) break;
	}
}

void hdl(int sig)
{
	kill_chlds();
}

int runpiped(execargs_t** programs, size_t n)
{
	if (n == 0)
	{
		write_(STDOUT_FILENO, "Oops, empty string!\n", 20);
		return 0;
	}    
	pidn = n;
	pids = malloc(n * sizeof(pid_t));
	if (pids == NULL) return -1;
	int i;
	for (i = 0; i < n; i++)
	{
		pids[i] = 0;
	}
	int pipefds[2];
	int in[n];
	in[0] = STDIN_FILENO;
	int out[n];
	out[n - 1] = STDOUT_FILENO;
	for (i = 0; i < n - 1; i++)
	{
		if (pipe(pipefds) == -1)
		{
			return -1;
		}
		out[i] = pipefds[1];
		in[i + 1] = pipefds[0];
	}	
	for (i = n - 1; i >= 0; i--)
	{
		int pid = fork();		
		if (pid == -1)
		{
			kill_chlds();
			wait_all();
			return -1;	
		}
		if (pid == 0)
		{
			if (dup2(in[i], STDIN_FILENO) == -1) exit(EXIT_FAILURE);
			if (dup2(out[i], STDOUT_FILENO) == -1) exit(EXIT_FAILURE);
			struct sigaction act;
    		memset(&act, 0, sizeof(act));
    		act.sa_handler = hdl;
    		sigset_t set; 
    		sigemptyset(&set);        
    		sigaddset(&set, SIGPIPE);
    		act.sa_mask = set;
			sigaction(SIGPIPE, &act, 0);
			exec(programs[i]);
		}
		else
		{
			pids[i] = pid;
		}
	}
	int status;
	wait(&status);
	kill_chlds();
	wait_all();
	return 0;  
}
