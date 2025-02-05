#ifndef __FILEMAN_H__
#define __FILEMAN_H__

#include <sys/stat.h>
#include <stdint.h>

struct mapped_file {
    uint64_t size;
    void* file;
    const char* name;
    uint64_t offset;
};

int file_exists(const char* file, struct stat* details);
int file_open(const char* file, int flags);
void file_close(int fd);
struct mapped_file* file_map(const char* file);

#endif