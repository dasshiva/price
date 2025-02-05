#ifndef __FILEMAN_H__
#define __FILEMAN_H__

#include <sys/stat.h>

int file_exists(const char* file, struct stat* details);
int file_open(const char* file, int flags);
void file_close(int fd);
void* file_map(const char* file);

#endif