#pragma once

#include "linmath.h"
#include <float.h>

struct bbox
{
    vec3 min;
    vec3 max;
};

static inline void bbox_init(struct bbox* bb)
{
    if (!bb) return;

    bb->min[0] = bb->min[1] = bb->min[2] = FLT_MAX;
    bb->max[0] = bb->max[1] = bb->max[2] = -FLT_MAX;
}
