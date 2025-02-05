#ifndef __CLASSFILE_H__
#define __CLASSFILE_H__
#include <stdint.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

struct classfile {
    u16 major;
};

struct classfile* parse_class(struct mapped_file* file);

#endif