#include "fileman.h"
#define SUBSYSTEM "core/fileman"
#include "logging.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

extern int errno;

int file_exists(const char* file, struct stat* details) {
    struct stat st;
    int err = stat(file, &st);
    if (err < 0) {
        if (errno == ENOENT) {
            warn("File %s does not exist", file);
        }
        else
            warn("File %s could not be accessed on account of %s", file, 
                    strerror(errno));

        return 0;
    }

    if (details) 
        memcpy(details, &st, sizeof(struct stat));

    return 1;
}

int file_open(const char* file, int flags) {
    log("Trying to open/create file %s", file);

    int exists = file_exists(file, NULL);
    if (!exists && ((flags & O_CREAT) == 0)) {
        warn("File %s does not exist and will not be created", file);
        return 0;
    }

    int fd = open(file, flags);
    if (fd < 0)
        return 0;
    return fd;
}

void file_close(int fd) {
    close(fd);
}

void* file_map(const char* file) {
    void* ret = NULL;
    struct stat st;
    if (!file_exists(file, &st))
        return NULL;

    int fd = file_open(file, O_RDONLY);
    if (!fd)
        return NULL;

    ret = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ret == MAP_FAILED) {
        warn("File %s could not be mapped to memory because %s", file, strerror(errno));
        return NULL;
    }

    return ret;
}