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

static inline void bbox_union(struct bbox *r, struct bbox *a, struct bbox *b)
{
    r->min[0] = fmin(a->min[0], b->min[0]);
    r->min[1] = fmin(a->min[1], b->min[1]);
    r->min[2] = fmin(a->min[2], b->min[2]);
    r->max[0] = fmax(a->max[0], b->max[0]);
    r->max[1] = fmax(a->max[1], b->max[1]);
    r->max[2] = fmax(a->max[2], b->max[2]);
}
