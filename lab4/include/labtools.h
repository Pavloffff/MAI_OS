#ifndef __LABTOOLS_H_
#define __LABTOOLS_H_

#include <string>
#include <sys/stat.h>

std::string backFile = "main.back";
std::string semFile = "main.semaphore";
int accessPerm = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

#define CHECK_ERROR(expr, err, message) \
    do { \
        auto __result = (expr); \
        if (__result == err) { \
            fprintf(stderr, "Error: %s\n", message); \
            fprintf(stderr, "errno = %s, file %s, line %d\n", strerror(errno), \
                    __FILE__, __LINE__); \
            exit(-1); \
        } \
    } while (0)

#endif