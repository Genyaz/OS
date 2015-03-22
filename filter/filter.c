#include "filter.h"
#include <stdlib.h>
#define BUF_SIZE 4096

void check(char str[BUF_SIZE], int str_len, char * args[])
{
	if (str_len > 0)
	{
		str[str_len] = 0;
		if (spawn(args[0], args) == 0)
		{
			str[str_len] = '\n';
			str[str_len + 1] = 0;		
			write_(STDOUT_FILENO, str, str_len + 1);
		}
	}
}

int main(int argc, char * const argv[])
{
	char * args[argc + 1];
	int i;
	for (i = 1; i < argc; i++)
	{
		args[i - 1] = argv[i];
	}
	char buf[BUF_SIZE];
	char str[BUF_SIZE];
	args[argc - 1] = str;
	args[argc] = 0;
	int buf_len = 0;
	while (1)
	{
		int read = read_(STDIN_FILENO, buf + buf_len, BUF_SIZE - buf_len);
		if (read == -1)
		{
			exit(EXIT_FAILURE);
		}
		else if (read == 0)
		{
			args[argc - 1] = buf;
			check(buf, buf_len, args);
			exit(EXIT_SUCCESS);
		}
		else
		{
			int buf_end = buf_len + read;
			int str_len = 0;
			for (i = 0; i < buf_end; i++)
			{
				if (buf[i] == '\n')
				{
					check(str, str_len, args);
					str_len = 0;
				}
				else
				{
					str[str_len++] = buf[i];
				}
			}
			if (str_len == BUF_SIZE)
			{
				exit(EXIT_FAILURE);
			}
			buf_len = str_len;
			for (i = 0; i < buf_len; i++)
			{
				buf[i] = str[i];
			}
		}
	}
}
