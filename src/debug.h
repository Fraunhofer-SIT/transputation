#pragma once

#ifdef _DEBUG
#include <stdio.h>
#define DEBUG(...) \
    do { \
        fprintf(stderr, "DEBUG:%s:%u: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
    } while (false)
#else
#define DEBUG(...)
#endif
