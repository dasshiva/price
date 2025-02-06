#ifndef __CONSTANT_POOL_H__
#define __CONSTANT_POOL_H__

#include "../fileman.h"
#include "classfile.h"

/*
 * The VM spec only uses bits 0-4 of the tag (at least as of Java 23)
 * This leaves bits 5-7 of the tag empty for our use
 * In order to accomodate future extensions to the constant pool tag values
 * We only use bits 6-7 to express the size in bytes (in powers of 2) of the data field.
 * This means that if t = tag[7:6] then this what they mean:
 * 0 - special case for CONSTANT_UTF8/CONSTANT_METHODHANDLE use the tag value to figure out
 * 1 - data is 2 bytes long
 * 2 - data is 4 bytes long
 * 3 - data is 8 bytes long (CONSTANT_LONG & CONSTANT_DOUBLE)
*/
struct constant_pool {
    u8 tag;
    void* data;
};

int validate_cpool(struct constant_pool* cpool);
struct constant_pool* parse_cpool(struct mapped_file* src, int len);

// Constant pool tags
#define CONSTANT_UTF8                1
#define CONSTANT_INTEGER             3
#define CONSTANT_FLOAT               4
#define CONSTANT_LONG                5
#define CONSTANT_DOUBLE              6
#define CONSTANT_CLASS               7
#define CONSTANT_STRING              8
#define CONSTANT_FIELDREF            9
#define CONSTANT_METHODREF           10
#define CONSTANT_INTERFACE_METHODREF 11
#define CONSTANT_NAMETYPE            12
#define CONSTANT_METHODHANDLE        15
#define CONSTANT_METHODTYPE          16
#define CONSTANT_INVOKEDYNAMIC       18

#endif