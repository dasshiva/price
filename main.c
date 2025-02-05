#define SUBSYSTEM "core/main"
#include "logging.h"
#include "fileman.h"
 
#define VM_VERSION "0.0.1"

int main(int argc, const char** argv) {
        log("Starting VM version %s", VM_VERSION);
        if (argc < 2) 
            error_with_code(1, "USAGE: %s [file].class", argv[0]);
        void* ret = file_map(argv[1]);
        if (!ret) 
            error_with_code(2, "Failed to initialise VM");
        return 0;
}