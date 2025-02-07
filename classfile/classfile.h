#ifndef __CLASSFILE_H__
#define __CLASSFILE_H__
#include <stdint.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

struct constant_pool;
struct mapped_file;

struct field_or_method {
    u16   acc_flags;
    u16   name;
    u16   desc;
    u16   attrs_count;
    void* attributes_base;
};

struct classfile {
    u16  major;
    u16  cpool_count;
    struct constant_pool*    cpool;
    u16   acc_flags;
    u16   this_class;
    u16   super_class;
    u16   interfaces_count;
    u16*  interfaces;
    u16   fields_count;
    struct field_or_method*  fields;
    u16   methods_count;
    struct field_or_method*  methods;
    u16   attrs_count;
    void* attributes_base;
};

struct classfile* parse_class(struct mapped_file* file);
struct field_or_method* parse_fields(struct mapped_file* src, struct constant_pool* cpool, int count, int cp_count);
struct field_or_method* parse_methods(struct mapped_file* src, struct constant_pool* cpool, int count, int cp_count);
#endif