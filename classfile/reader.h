#ifndef __READER_H__
#define __READER_H__

#include "classfile.h"
#include "../fileman.h"

u8 read_u8(struct mapped_file* src);
u16 read_u16(struct mapped_file* src);
u32 read_u32(struct mapped_file* src);
u64 read_u64(struct mapped_file* src);
int skip(uint64_t off, struct mapped_file* src);

#endif