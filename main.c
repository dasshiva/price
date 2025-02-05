#define SUBSYSTEM "core/main"
#include "logging.h"
 
#define VM_VERSION "0.0.1"

int main(int argc, const char** argv) {
        log("Starting VM version %s", VM_VERSION);
        return  0;
}