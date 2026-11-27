#ifndef TEST_CHECK_H
#define TEST_CHECK_H

#include <cstdio>

extern int g_failures;   // 定义在 tests_main.cpp

#define CHECK(cond)                                                          \
    do                                                                       \
    {                                                                        \
        if (!(cond))                                                         \
        {                                                                    \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

#endif // TEST_CHECK_H
