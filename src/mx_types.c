#include "src/mx_types.h"

v8si_t v8si_zero = { .val = {0, 0, 0, 0, 0, 0, 0, 0} };
v8si_t v8si_mask[9] = {
    { .val = { 0,  0,  0,  0,  0,  0,  0,  0} },
    { .val = {~0,  0,  0,  0,  0,  0,  0,  0} },
    { .val = {~0, ~0,  0,  0,  0,  0,  0,  0} },
    { .val = {~0, ~0, ~0,  0,  0,  0,  0,  0} },
    { .val = {~0, ~0, ~0, ~0,  0,  0,  0,  0} },
    { .val = {~0, ~0, ~0, ~0, ~0,  0,  0,  0} },
    { .val = {~0, ~0, ~0, ~0, ~0, ~0,  0,  0} },
    { .val = {~0, ~0, ~0, ~0, ~0, ~0, ~0,  0} },
    { .val = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0} },
};

v8sf_t v8sf_zero = { .val = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} };
v8sf_t v8sf_mask[9] = {
    { .val = { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0,  0.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0,  0.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0,  0.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  0.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  0.0} },
    { .val = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0} },
};