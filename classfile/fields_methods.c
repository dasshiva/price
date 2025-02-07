#include "classfile.h"
#include "../fileman.h"
#include "reader.h"
#include <stdlib.h>

#define SUBSYSTEM "classfile/fields-methods"
#include "../logging.h"

static void parse_headers(struct field_or_method* fm, struct mapped_file* src, struct constant_pool* cpool) {
    fm->acc_flags = read_u16(src);
    fm->name = read_u16(src);
    fm->desc = read_u16(src);
    fm->attrs_count = read_u16(src);
}

struct field_or_method* parse_fields(struct mapped_file* src, struct constant_pool* cpool, int count) {
     log("Parsing %d fields", count);
    struct field_or_method* ret = NULL;
    if (!count)
        return NULL;

    ret = malloc(sizeof(struct field_or_method) * count);
    for (int i = 0; i < count; i++) {
        parse_headers(&ret[i], src, cpool);
    }

    return ret;
}

struct field_or_method* parse_methods(struct mapped_file* src, struct constant_pool* cpool, int count) {
    log("Parsing %d methods", count);
    struct field_or_method* ret = NULL;
    if (!count)
        return NULL;

    ret = malloc(sizeof(struct field_or_method) * count);
    for (int i = 0; i < count; i++) {
        parse_headers(&ret[i], src, cpool);
    }

    return ret;
}