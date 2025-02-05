#ifndef __LOGGING_H__
#define __LOGGING_H__

// Values for level
#define LOG                        0
#define WARN                       1
#define ERROR                      2
#define ERROR_WITH_EXIT_CODE(code) (2 + code)

// Do not use this function directly, call the macros defined below
int logger(const char*, const char*, int, const char*, int, const char*, ...);

#ifndef SUBSYSTEM 
#error  "SUBSYSTEM macro is undeclared"
#endif

#define log(...) \
    logger(SUBSYSTEM, __FILE__, __LINE__, __func__, LOG, __VA_ARGS__);
#define warn(...) \
    logger(SUBSYSTEM, __FILE__, __LINE__, __func__, WARN, __VA_ARGS__);
#define error(...) \
    logger(SUBSYSTEM, __FILE__, __LINE__, __func__, ERROR, __VA_ARGS__);
#define error_with_code(code, ...) \
    logger(SUBSYSTEM, __FILE__, __LINE__, __func__, ERROR_WITH_EXIT_CODE(code), __VA_ARGS__);
#endif