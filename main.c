#include <bits/types/siginfo_t.h>
#include <signal.h>
#include <stdlib.h>
#define SUBSYSTEM "core/main"
#include "logging.h"
#include "fileman.h"
#include "classfile/classfile.h"
 
#define VM_VERSION "0.0.1"

void handler(int sig, siginfo_t* info, void* ucontext) {
    if (sig == SIGSEGV) {
        error_with_code(4, "Access fault at address %p detected", info->si_addr);
    }
    else if (sig == SIGUSR1) {
        error_with_code(3, "Internal error: Class file exhausted while reading");
    }
    else if (sig == SIGTERM || sig == SIGQUIT || sig == SIGINT) {
        error("Termination signal received from process %d", info->si_addr);
    }
    else 
        warn("Unexpected signal %d received from process %d", sig, info->si_pid);
    
}

int main(int argc, const char** argv) {
        struct sigaction sa = {0};
        sa.sa_sigaction = &handler;
        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGSEGV, &sa, NULL);
        log("Starting VM version %s", VM_VERSION);
        if (argc < 2) 
            error_with_code(1, "USAGE: %s [file].class", argv[0]);
        struct mapped_file* ret = file_map(argv[1]);
        if (!ret) 
            error_with_code(2, "Failed to initialise VM because main class could not be read");

        log("File %s (%ld bytes) sucessfully mapped", argv[1], ret->size);
        struct classfile* class = parse_class(ret);
        if (!class) {
            const char* n = ret->name;
            free(ret);
            error_with_code(3, "Failed to parse class file %s", n);
        }
        return 0;
}