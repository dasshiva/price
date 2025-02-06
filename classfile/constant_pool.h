#ifndef __CONSTANT_POOL_H__
#define __CONSTANT_POOL_H__

#include "../fileman.h"
#include "classfile.h"
struct constant_pool {
    
};

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