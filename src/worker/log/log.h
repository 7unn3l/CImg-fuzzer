#pragma once
#include <cstdio>

#ifdef fuzzer_log

#include <ctime>

// logging via printf over << because i like it better. Not thread safe
#define LOG(...) printf("[%15s:%-4d] ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");

#endif