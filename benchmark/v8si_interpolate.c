#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>

#include "src/mx_utils.h"

int main(int argc, const char * argv[])
{
    struct timeval begin = {0};
    struct timeval end = {0};
    long long diff = 0;
    int i = 0;
    int itr = 1000;
    int32_t xp[] = {1,  5, 10, 12};
    int32_t fp[] = {3, 17, 35, 42};
    float slp[] = {3.5, 3.5, 3.5, 3.5};
    int32_t x[] = {4, 8, 10, 1, 12, -1, 13, 7};
    int32_t y[sizeof(x) / sizeof(x[0])];

    if (argc > 1) {
        itr = atoi(argv[1]); 
    } // if

    printf("Search 8 values simultaneously, iterate for %d times.\n", itr);
    printf("Calculating using simd code.\n");
    gettimeofday(&begin, NULL);
    for (i = 0; i < itr; i += 1) {
        mx_v8si_interpolate(y, sizeof(xp) / sizeof(xp[0]), xp, fp, slp, NULL, NULL, sizeof(x) / sizeof(x[0]), x);
    } // for
    gettimeofday(&end, NULL);
    diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    printf("Time for mx_v8si_interpolate() using SIMD code = %7.6fs\n", (float)diff / 1000000);
}
