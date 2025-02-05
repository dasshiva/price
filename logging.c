#define  SUBSYSTEM "core/log"

#include "logging.h"
#include <stdio.h>

/* The most important for us here is a good logging system
 * This helps us understand what the code is doing at any point 
 * making bug spotting easier and just making it overall easier for
 * anyone to understand the code
 *
 * Make logging as verbose as possible by including line number, file name
 * and function name. Log explicitly which susbystem is logging so it is
 * easier to grep through logs and find relevant information.
 *
 * Remember: There is no such thing as logging "too much".
 */

#include <stdarg.h>
#include <stdlib.h>

static const char* level_to_text[] = {"LOG", "WARN", "ERROR"};

int logger(const char* susbystem, const char* file, int line, 
            const char* func, int level, const char* msg, ...) {
        va_list list;
        va_start(list, msg);
        int exit_code = 0;

        if (level > ERROR) {
            exit_code = level - ERROR;
            level -= exit_code;
        }
#ifdef DEBUG
        fprintf(stdout, "%s: %s:%s:%d %s() ", level_to_text[level], susbystem, 
                    file, line, func);
        vfprintf(stdout, msg, list);
        fprintf(stdout, "\n");
#endif

        if (level == ERROR) {
#ifndef DEBUG
            vfprintf(stdout, msg, list);
            fprintf(stdout, "\n");
#endif
            exit(exit_code);
        }

        va_end(list);
        return 0;
}