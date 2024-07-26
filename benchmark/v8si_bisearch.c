#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>

#include "src/mx_utils.h"

static int32_t default_range[] = {0, 5, 10, 15, 20, 20, 20, 25, 30, 35, 40, 45, 50};

uint32_t collect_range(int32_t ** out)
{
    struct timeval waittime = {1, 0};
    fd_set fs;
    int ret = 0;
    int32_t * range = NULL;
    uint32_t cap = 0;
    uint32_t cnt = 0;

    FD_ZERO(&fs);
    FD_SET(0, &fs);
    ret = select(1, &fs, NULL, NULL, &waittime);
    if (ret < 0 && ret != EAGAIN) {
        printf("Something bad happened when reading stdin.\n");
        abort();
    } else if (ret == 0) {
        *out = default_range;
        return sizeof(default_range) / sizeof(default_range[0]);
    } // if

    cap = 1024;
    range = calloc(sizeof(int32_t), cap);
    if (! range) {
        printf("Not enough memory!\n");
        abort();
    } // if

    do {
        do {
            ret = fscanf(stdin, "%d\n", &range[cnt]);
            if (ret > 0) {
                cnt += 1;
                if (cnt == cap) {
                    range = realloc(range, sizeof(int32_t) * (cap + (cap >> 1)));
                    if (! range) {
                        printf("Not enough memory!\n");
                        abort();
                    } // if
                    cap += (cap >> 1);
                } // if
            } else if (ret == EOF) {
                *out = range;
                return cnt;
            } else {
                break;
            } // if
        } while (1);

        FD_ZERO(&fs);
        FD_SET(0, &fs);
        ret = select(1, &fs, NULL, NULL, &waittime);
        if (ret < 0 && ret != EAGAIN) {
            printf("Something bad happened when reading stdin.\n");
            abort();
        } else if (ret == 0) {
            break;
        } // if
    } while (ret > 0 && FD_ISSET(0, &fs));

    *out = range;
    return cnt;
} // collect_range

int main(int argc, const char * argv[])
{
    struct timeval begin = {0};
    struct timeval end = {0};
    long long diff = 0;
    int i = 0;
    int itr = 1000;
    int32_t * range = NULL;
    uint32_t range_len = 0;
    v8si_t idx;
    v8si_t src = { .val = {9, 50, 37, 1, 25, 60, -5, 22} };

    if (argc > 1) {
        itr = atoi(argv[1]); 
    } // if

    range_len = collect_range(&range);

    printf("Search 8 values simultaneously, iterate for %d times.\n", itr);
    if (range == default_range) {
        printf("Search range is: \n");
        printf("posi\t", i);
        for (i = 0; i < range_len; i += 1) {
            printf("%2d\t", i);
        } // for
        printf("\n");
        printf("value\t", i);
        for (i = 0; i < range_len; i += 1) {
            printf("%2d\t", range[i]);
        } // for
        printf("\n");
    } else {
        printf("Search range has %d values.\n", range_len);
    } // if

    printf("Calculating using simd code.\n");
    gettimeofday(&begin, NULL);
    for (i = 0; i < itr; i += 1) {
        mx_v8si_bisearch(&idx, range, range_len, &src);
    } // for
    gettimeofday(&end, NULL);
    diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
    printf("Time for mx_v8si_bisearch() using SIMD code = %7.6fs\n", (float)diff / 1000000);

    for (i = 0; i < 8; i += 1) {
        printf("target=%4d\tindex=%4d\tvalue=%4d\n", mx_type_val(src)[i], mx_type_val(idx)[i], range[ mx_type_val(idx)[i] ]);
    } // for
}
