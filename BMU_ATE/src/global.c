/*
*
*
*
*
*
*/

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../inc/global.h"

int get_module_file_name(char* buf, int len)
{
	if (get_module_file_path_name(buf, len))
	{
		int l = strlen(buf);
		const char* p = buf + l;
		while(*p != '/')
			-- p;
		++ p;
		len = 0;
		while(*p != 0)
		{
			*buf = *p;
			++ p;
			++ len;
			++ buf;
		}
		*buf = 0;
		return len;
	}
	return -1;
}

char* get_module_file_path_name(char* buf, int len)
{
	const char* link = "/proc/self/exe";
	memset(buf, 0, len);
	if (readlink(link, buf, (size_t)len) != -1)
	{
		return buf;
	}
	return NULL;
}

int create_dir(const char* path_name, int mode)
{
	struct stat file_info;
	if (stat(path_name, &file_info) == -1)
	{
		return mkdir(path_name, mode);
	}
	return 0;
}

int is_dir(const char* path_name)
{
	struct stat file_info;
	if (stat(path_name, &file_info) == -1)
	{
		return -1;
	}
	return 0;
}

int is_file(const char* path_name)
{
	if (access(path_name, F_OK) < 0)
		return -1;
	return 0;
}
