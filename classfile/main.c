#include "../fileman.h"
#include "classfile.h"
#include <stdlib.h>

#define SUBSYSTEM "classfile/main"
#include "../logging.h"

struct classfile* parse_class(struct mapped_file* file) {
    struct classfile* class = NULL;
    log("Beginning to parse classfile %s", file->name);
    
    return class;
}