#include "../fileman.h"
#include "classfile.h"
#include "reader.h"
#include <stdlib.h>

#define CLASS_MAGIC 0xCAFEBABE 
#define MAX_JAVA_VERSION 48
#define SUBSYSTEM "classfile/main"
#include "../logging.h"
#include "constant_pool.h"

struct classfile* parse_class(struct mapped_file* file) {
    struct classfile* class = NULL;
    log("Beginning to parse classfile %s", file->name);
    if (read_u32(file) != CLASS_MAGIC) {
        warn("Class file %s has invalid magic", file->name);
        return NULL;
    }

    u16 minor = read_u16(file);
    u16 major = read_u16(file);
    if (minor != 0 || major > MAX_JAVA_VERSION) {
        warn("Unsupported class file version %d.%d", major - 44, minor);
        return NULL;
    }
    log("Parsing class file version %d.%d", major - 44, minor);

    class = malloc(sizeof(struct classfile));
    class->major = major;
    class->cpool_count = read_u16(file);
    log("Constant pool count = %d", class->cpool_count);
    class->cpool = parse_cpool(file, class->cpool_count);
    if (!class->cpool) {
        warn("Failed to parse constant pool");
        goto fail;
    }

    return class;

fail:
    free(class);
    return NULL;
}