#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "src/i32_matrix.h"

int main(int argc, const char * argv[])
{
    struct timeval begin = {0};
    struct timeval end = {0};
    long long diff = 0;
    int i = 0;
    int j = 0;
    int cnt = 1000;
    int n = 256;
    unsigned int code = 0;
    i32_matrix_ptr lhs = NULL;
    i32_matrix_ptr rhs = NULL;
    i32_matrix_ptr mx = NULL;

    if (argc > 1) {
        cnt = atoi(argv[1]); 
    } // if

    if (argc > 2) {
        n = atoi(argv[2]); 
    } // if

    lhs = i32m_allocate(n, n);
    rhs = i32m_allocate(n, n);
    mx = i32m_allocate_for_multiplying(lhs, rhs);

    printf("Multiply %dx%d matrices, iterate for %d times.\n", n, n, cnt);

    i32m_init_ones(lhs);
    i32m_init_ones(rhs);

    printf("Calculating using simd code.\n");
    gettimeofday(&begin, NULL);
    for (i = 0; i < cnt; i += 1) {
        i32m_multiply_and_store(mx, lhs, rhs);
    } // for
    gettimeofday(&end, NULL);
    diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    printf("Time for mtx_multiply_and_store() using SIMD code = %7.6fs\n", (float)diff / 1000000);

    int32_t ret = i32m_get(mx, 0, 0);
    printf("(0,0)=%d\n", ret);

    for (i = 0; i < n; i += 1) {
        for (j = 0; j < n; j += 1) {
            if (ret != i32m_get(mx, i, j)) {
                printf("(%d,%d)=%d\n", i, j, i32m_get(mx, i, j));
            } // if
        } // for
    } // for

    i32m_destroy(mx);
    i32m_destroy(rhs);
    i32m_destroy(lhs);
    return 0;
}
