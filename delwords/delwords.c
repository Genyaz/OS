#include "helpers.h"
#include <string.h>
#include <stdlib.h>
#define buf_size 4

int main(int argc, char const * argv[])
{
	char buf[buf_size];
	char write_buf[buf_size];	
	char const * word = argv[1];
	size_t len = strlen(word);
	int prev_read = 0;
	while (1)
	{
		int read = read_(STDIN_FILENO, buf + prev_read, buf_size - prev_read);
		if (read == -1)
		{
			exit(EXIT_FAILURE);
		}
		else if (read == 0)
		{
			write_(STDOUT_FILENO, buf, prev_read);
			exit(EXIT_SUCCESS);
		}
		else
		{
			int buf_end = prev_read + read;
			int i = 0;
			int to_write = 0;
			while (i < buf_end - len + 1)
			{
				int equal = 1;
				int j = 0;
				while (equal && j < len)
				{
					equal = equal && (buf[i + j] == word[j]);
					j++;
				}
				if (equal)
				{
					i += len;
				}
				else
				{
					write_buf[to_write++] = buf[i];
					i++;
				}
			}
			int shift = i;
			while (i < buf_end)
			{
				buf[i - shift] = buf[i];
				i++;
			}
			prev_read = buf_end - shift;
			write_(STDOUT_FILENO, write_buf, to_write);
		}
	}
}
