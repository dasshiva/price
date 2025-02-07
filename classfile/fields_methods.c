#include "classfile.h"
#include "constant_pool.h"
#include "../fileman.h"
#include "reader.h"
#include <stdint.h>
#include <stdlib.h>

#define SUBSYSTEM "classfile/fields-methods"
#include "../logging.h"

#define IS_VALID_INDEX(idx, len) (idx < len)

static int parse_headers(struct field_or_method* fm, struct mapped_file* src, struct constant_pool* cpool, int cp_count) {
    fm->acc_flags = read_u16(src);
    
    fm->name = read_u16(src) - 1;
    if (!IS_VALID_INDEX(fm->name, cp_count)) {
        warn("Field/method name index %d is invalid", fm->name);
        return 0;
    }

    if (cpool[fm->name].tag != CONSTANT_UTF8) {
        warn("Field/method name index %d does not refer to a UTF-8 string", fm->name);
        return 0;
    }

    log("field/method name index = %d", fm->name);

    fm->desc = read_u16(src) - 1;
    if (!IS_VALID_INDEX(fm->desc, cp_count)) {
        warn("Field/method descriptor index %d is invalid", fm->desc);
        return 0;
    }

    if (cpool[fm->name].tag != CONSTANT_UTF8) {
        warn("Field/method descriptor index %d does not refer to a UTF-8 string", fm->desc);
        return 0;
    }

    log("field/method descriptor index = %d", fm->desc);

    fm->attrs_count = read_u16(src);
    log("Field/method attributes count = %d", fm->attrs_count);

    fm->attributes_base = ((uint8_t*)src->file) + src->offset;
    return 1;
}

struct field_or_method* parse_fields(struct mapped_file* src, struct constant_pool* cpool, int count, int cp_count) {
    log("Parsing %d fields", count);
    struct field_or_method* ret = NULL;
    if (!count)
        return NULL;

    ret = malloc(sizeof(struct field_or_method) * count);
    for (int i = 0; i < count; i++) {
        parse_headers(&ret[i], src, cpool, cp_count);
    }

    return ret;
}

struct field_or_method* parse_methods(struct mapped_file* src, struct constant_pool* cpool, int count, int cp_count) {
    log("Parsing %d methods", count);
    struct field_or_method* ret = NULL;
    if (!count)
        return NULL;

    ret = malloc(sizeof(struct field_or_method) * count);
    for (int i = 0; i < count; i++) {
        parse_headers(&ret[i], src, cpool, cp_count);
    }

    return ret;
}