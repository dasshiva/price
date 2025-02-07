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
    log("Parsing constant pool of %d elements", len - 1);
    struct constant_pool* pool = malloc(sizeof(struct constant_pool) * len);
    int index = 0;
    while (index < len - 1) {
        pool[index].tag = read_u8(src);
        pool[index].data = raw_buf + src->offset;
        log("Constant pool index = %d tag = %d", index + 1, pool[index].tag);
        switch (pool[index].tag) {
            case CONSTANT_UTF8: {
                u16 size = read_u16(src);
                log("UTF-8 String of length = %d bytes", size);

                if(!putback(2, src)) {
                    warn("Internal error: Cannot rewrite string length");
                    return NULL;
                }

                write_u16(size, src);
                if (!skip(size, src)) {
                    warn("Malformed file: String extends till end of file");
                    return NULL;
                }
                break;
            }
            
            case CONSTANT_FIELDREF:
            case CONSTANT_METHODREF:
            case CONSTANT_NAMETYPE:
            case CONSTANT_INTERFACE_METHODREF:
            case CONSTANT_INVOKEDYNAMIC: {
                u16 d1 = read_u16(src);
                d1 -= (pool[index].tag == CONSTANT_INVOKEDYNAMIC) ? 0 : 1;
                u16 d2 = read_u16(src);
                d2 -= 1;
                if (!putback(4, src)) {
                    warn("Cannot rewrite constant pool indexes");
                    return NULL;
                }

                write_u16(d1, src);
                write_u16(d2, src);

                log("field/method/interface/nametype/invdyn reference to class/bootstrap_method/name = %d signature/descriptor = %d", 
                            d1 + ((pool[index].tag == CONSTANT_INVOKEDYNAMIC) ? 0 : 1), d2 + 1);
                pool[index].tag |= (1 << 7); // Data is 4 bytes
                break;
            }

            case CONSTANT_INTEGER: 
            case CONSTANT_FLOAT: {
                pool[index].tag |= (1 << 7); // Data is 4 bytes
                u32 bytes = read_u32(src);
                if (!putback(4, src)) {
                    warn("Cannot rewrite integer element");
                    return NULL;
                }

                write_u32(bytes, src);
                log("Integer/Float with value = %d (ignore value if tag is 4)", (int)bytes);
                break;
            }

            case CONSTANT_LONG: 
            case CONSTANT_DOUBLE: {
                pool[index].tag |= (1 << 7) | (1 << 6); // Data is 8 bytes
                u64 hi = read_u32(src);
                u64 lo = read_u32(src);

                if (!putback(8, src)) {
                    warn("Cannot rewrite integer element");
                    return NULL;
                }

                write_u32(hi, src);
                write_u32(lo, src);
                log("Long/Double with value = %d (ignore value if tag is 6)", (long)((hi << 32) | (lo)));

                index++; // long and double values occupy two indexes in the constant pool
                break;
            }

            case CONSTANT_STRING:
            case CONSTANT_METHODTYPE:
            case CONSTANT_CLASS: {
                pool[index].tag |= (1 << 6); // Data is 2 bytes long
                u16 class = read_u16(src);
                class -= 1; // constant pool indexes are 1-based
                log("Class/String/methodtype reference to index = %d", class + 1);
                if(!putback(2, src)) {
                    warn("Internal error: Cannot rewrite class/string/methodtype ref");
                    return NULL;
                }

                write_u16(class, src);
                break;
            }

            case CONSTANT_METHODHANDLE: {
                u8 kind = read_u8(src);
                if (kind < 1 || kind > 9) {
                    warn("Invalid method handle kind");
                    return NULL;
                }

                u16 ref_index = read_u16(src);
                ref_index -= 1;
                 if(!putback(2, src)) {
                    warn("Internal error: Cannot rewrite method handle ref");
                    return NULL;
                }

                write_u16(ref_index, src);
                break;
            }

            default:
                warn("Unknown constant pool element with tag = %d", pool[index].tag);
                return NULL;
        }

        index++;
    }

    return pool;
}

#define IS_VALID_INDEX(idx, len) (idx < len)
#define TAG(tag) (tag & 0b11111)

int validate_cpool(struct constant_pool* cpool, int len) {
    int index = 0;
    while (index < len - 1) {
        struct constant_pool elem = cpool[index];
        int tag = elem.tag & 0b11111;
        log("Validating constant pool index =  %d", index);
        switch (tag) { // get lowest 5 bits
            case CONSTANT_FIELDREF:
            case CONSTANT_METHODREF:
            case CONSTANT_INTERFACE_METHODREF: {
                struct mapped_file src = {0};
                src.file = elem.data;
                src.offset = 0;
                src.size = 4;
                u16 class = read_u16_nr(&src);
                u16 ntype = read_u16_nr(&src);

                if (!IS_VALID_INDEX(class, len)) {
                    warn("class index %d in field/method/interface ref is out-of-bounds", class);
                    return 0;
                }

                if (!IS_VALID_INDEX(ntype, len)) {
                     warn("name-type index %d in field/method/interface ref is out-of-bounds", ntype);
                    return 0;
                }

                if (TAG(cpool[class].tag) != CONSTANT_CLASS) {
                    warn("class index %d does not refer to CONSTANT_CLASS", class);
                    return 0;
                }

                if (TAG(cpool[class].tag) != CONSTANT_NAMETYPE) {
                    warn("name-type index %d does not refer to CONSTANT_NAMETYPE", ntype);
                    return 0;
                }

                break;
            }

            case CONSTANT_STRING:
            case CONSTANT_METHODTYPE:
            case CONSTANT_CLASS: {
                struct mapped_file src = {0};
                src.file = elem.data;
                src.offset = 0;
                src.size = 2;

                u16 class = read_u16_nr(&src);

                if (!IS_VALID_INDEX(class, len)) {
                    warn("name index %d for string/methodtype/class is invalid", class);
                    return 0;
                }

                if (TAG(cpool[class].tag) != CONSTANT_UTF8) {
                    warn("name index %d for string/methodtype/class does not refer to CONSTANT_UTF8", class);
                }
                break;
            }

            case CONSTANT_NAMETYPE: {
                struct mapped_file src = {0};
                src.file = elem.data;
                src.offset = 0;
                src.size = 4;

                u16 name = read_u16_nr(&src);
                u16 desc = read_u16_nr(&src);

                if (!IS_VALID_INDEX(name, len)) {
                    warn("name index %d for name-type descriptor is invalid", name);
                    return 0;
                }

                if (!IS_VALID_INDEX(desc, len)) {
                    warn("desc index %d for name-type descriptor is invalid", desc);
                    return 0;
                }

                if (TAG(cpool[name].tag) != CONSTANT_UTF8) {
                    warn("name index %d for name-type descriptor does not refer to CONSTANT_UTF8", name);
                    return 0;
                }

                if (TAG(cpool[desc].tag) != CONSTANT_UTF8) {
                    warn("name index %d for name-type descriptor does not refer to CONSTANT_UTF8", desc);
                    return 0;
                }

                break;
            }

            case CONSTANT_DOUBLE:
            case CONSTANT_LONG: {
                index++;
                break;
            }

            default: {
                if (tag == CONSTANT_METHODHANDLE && tag == CONSTANT_INVOKEDYNAMIC) {
                    warn("Method handles and invoke dynamic are not supported");
                    return 0;
                }

                if (tag != CONSTANT_INTEGER && tag != CONSTANT_UTF8 && 
                    tag != CONSTANT_FLOAT) {
                        warn("Internal error: Unknown tag value %d", tag);
                        return 0;
                }
            } // We do not validate CONSTANT_INTEGER/UTF8/FLOAT/DOUBLE/LONG
        }
        index++;
    }
    return 1;
}
