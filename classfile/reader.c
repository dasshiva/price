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
    u16* buf = src->file;
    u16 data = buf[src->offset / sizeof(uint16_t)];
    src->offset += 2;
    return ntohs(data);
}

u32 read_u32(struct mapped_file* src) {
    u32* buf = src->file;
    u32 data = buf[src->offset / sizeof(uint32_t)];
    src->offset += 4;
    return ntohl(data);
}

u64 read_u64(struct mapped_file* src) {
    u64 d1 = read_u32(src);
    u64 d2 = read_u32(src);
    return (d2 << 32) | d1;
}
