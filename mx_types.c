#include "mx_types.h"

v8si_t v8si_zero = {0, 0, 0, 0, 0, 0, 0, 0};
v8si_t v8si_mask[9] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {~0,  0,  0,  0,  0,  0,  0,  0},
    {~0, ~0,  0,  0,  0,  0,  0,  0},
    {~0, ~0, ~0,  0,  0,  0,  0,  0},
    {~0, ~0, ~0, ~0,  0,  0,  0,  0},
    {~0, ~0, ~0, ~0, ~0,  0,  0,  0},
    {~0, ~0, ~0, ~0, ~0, ~0,  0,  0},
    {~0, ~0, ~0, ~0, ~0, ~0, ~0,  0},
    {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0}
};
