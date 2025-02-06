#include "constant_pool.h"

#define SUBSYSTEM "classfile/cpool"
#include "../logging.h"
#include <stdlib.h>
#include "classfile.h"
#include "reader.h"

struct constant_pool* parse_cpool(struct mapped_file* src, int len) {
    u8* raw_buf = src->file;
    // JVM spec 4.1 (under constant_pool_count) says 
    // "The value of the constant_pool_count item is equal to the number of entries in the constant_pool table plus one"
    // So the actual number of items is one less than len
    len -= 1;
    log("Parsing constant pool of %d elements", len);
    struct constant_pool* pool = (struct constant_pool*) raw_buf;
    int index = 0;
    while (index < len) {
        u8 tag = read_u8(src);
        log("Constant pool index = %d tag = %d", index + 1, tag);
        switch (tag) {
            default:
                warn("Unknown constant pool element with tag = %d", tag);
                return NULL;
        }
        index++;
        break;
    }
    return pool;
}
