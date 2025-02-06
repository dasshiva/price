#include "../fileman.h"
#include "classfile.h"
#include <arpa/inet.h>
#include <stdint.h>

u8 read_u8(struct mapped_file* src) {
    u8* buf = src->file;
    u8 data = buf[src->offset];
    src->offset++;
    return data;
}

u16 read_u16(struct mapped_file* src) {
    u16* buf = (uint16_t*) ((uint8_t*)src->file + src->offset);
    u16 data = *buf;
    src->offset += 2;
    return ntohs(data);
}

u32 read_u32(struct mapped_file* src) {
    u32* buf = (uint32_t*) ((uint8_t*)src->file + src->offset);
    u32 data = *buf;
    src->offset += 4;
    return ntohl(data);
}

u64 read_u64(struct mapped_file* src) {
    u64 d1 = read_u32(src);
    u64 d2 = read_u32(src);
    return (d1 << 32) | d2;
}

int skip(u64 off, struct mapped_file* src) {
    if (src->offset + off >= src->size)
        return 0;
    src->offset += off;
    return 1;
}

int putback(u64 off, struct mapped_file* src) {
    if (src->offset - off > src->offset) // detect underflow
        return 0;
    src->offset -= off;
    return 1;
}

void write_u8(u8 data, struct mapped_file* src) {
    u8* buf = src->file;
    buf[src->offset] = data;
    src->offset += 1;
}

void write_u16(u16 data, struct mapped_file* src) {
    u16* buf = src->file;
    buf[src->offset] = data;
    src->offset += 2;
}

void write_u32(u32 data, struct mapped_file* src) {
    u32* buf = src->file;
    buf[src->offset] = data;
    src->offset += 4;
}
