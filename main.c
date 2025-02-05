#define SUBSYSTEM "core/main"
#include "logging.h"
#include "fileman.h"
#include "classfile/classfile.h"
 
#define VM_VERSION "0.0.1"

int main(int argc, const char** argv) {
        log("Starting VM version %s", VM_VERSION);
        if (argc < 2) 
            error_with_code(1, "USAGE: %s [file].class", argv[0]);
        struct mapped_file* ret = file_map(argv[1]);
        if (!ret) 
            error_with_code(2, "Failed to initialise VM because main class could not be read");

        log("File %s (%ld bytes) sucessfully mapped", argv[1], ret->size);
        struct classfile* class = parse_class(ret);
        if (!class)
            error_with_code(3, "Failed to parse class file %s", ret->name);
        return 0;
}