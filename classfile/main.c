#include "../fileman.h"
#include "classfile.h"
#include "reader.h"
#include <stdlib.h>

#define CLASS_MAGIC 0xCAFEBABE 
#define MAX_JAVA_VERSION 52 // (Java 1.8)
#define SUBSYSTEM "classfile/main"
#include "../logging.h"
#include "constant_pool.h"

#define IS_VALID_INDEX(idx, len) (idx < len)

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
    class->cpool_count = read_u16(file) - 1;
    log("Constant pool count = %d", class->cpool_count);
    class->cpool = parse_cpool(file, class->cpool_count);
    if (!class->cpool) {
        warn("Failed to parse constant pool");
        goto fail;
    }

    if (!validate_cpool(class->cpool, class->cpool_count)) {
        warn("Constant Pool elements do not uphold all invariants");
        free(class->cpool);
        goto fail;
    }

    class->acc_flags = read_u16(file);
    log("Class access flags = %d", class->acc_flags);

    class->this_class = read_u16(file) - 1;
    if (!IS_VALID_INDEX(class->this_class, class->cpool_count)) {
        warn("this_class index %d is invalid", class->this_class);
        goto fail;
    }

    if ((class->cpool[class->this_class].tag & 0b11111) != CONSTANT_CLASS) {
        warn("this_class index %d does not refer to a CONSTANT_CLASS", class->this_class);
        goto fail;
    }
    
    log("this_class index = %d", class->this_class);

    class->super_class = read_u16(file);
    class->super_class -= (class->super_class == 0) ? 0 : 1;
    if (class->super_class != 0) { // this is not java/lang/Object
        if (!IS_VALID_INDEX(class->super_class, class->cpool_count)) {
            warn("super_class index %d is invalid", class->super_class);
            goto fail;
        }

        if ((class->cpool[class->super_class].tag & 0b11111) != CONSTANT_CLASS) {
            warn("super_class index %d does not refer to a CONSTANT_UTF8", class->super_class);
            goto fail;
        }

    }

    log("super_class index = %d", class->super_class);

    class->interfaces_count = read_u16(file);
    log("Interfaces count = %d", class->interfaces_count);

    class->interfaces = (u16*)((u8*)file->file + file->offset);
    if (!skip(sizeof(u16) * class->interfaces_count, file)) {
        warn("Malformed class file: Too many interfaces (%d interfaces)", class->interfaces_count);
        goto fail;
    }

    class->fields_count = read_u16(file);
    log("Fields count = %d", class->fields_count);

    class->fields = parse_fields(file, class->cpool, class->fields_count, class->cpool_count);
    if (!class->fields && (class->fields_count != 0)) {
        warn("Failed to parse the class fields");
        goto fail;
    }

    class->methods_count = read_u16(file);
    log("methods count = %d", class->methods_count);

    class->methods = parse_methods(file, class->cpool, class->methods_count, class->cpool_count);
    if (!class->methods && (class->methods_count != 0)) {
        warn("Failed to parse the class methods");
        goto fail;
    }
    return class;

fail:
    free(class);
    return NULL;
}
