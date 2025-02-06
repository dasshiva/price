#ifndef __CONSTANT_POOL_H__
#define __CONSTANT_POOL_H__

#include "../fileman.h"
#include "classfile.h"
struct constant_pool {
    
};

struct constant_pool* parse_cpool(struct mapped_file* src, int len);
#endif