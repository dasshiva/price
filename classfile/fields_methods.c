#include "classfile.h"
#include "../fileman.h"
#include <stdlib.h>

struct field_or_method* parse_fields(struct mapped_file* src, struct constant_pool* cpool, int count) {
    struct field_or_method* ret = NULL;
    if (!count)
        return NULL;

    
    return ret;
}

struct field_or_method* parse_methods(struct mapped_file* src, struct constant_pool* cpool, int count) {
    struct field_or_method* ret = NULL;
    if (!count)
        return NULL;
    return ret;
}