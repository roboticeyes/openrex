#include <math.h>

#include "camera.h"
#include "linmath.h"

void camera_init (struct camera *c, vec3 pos)
{
    if (!c) return;
    vec3 up = {0.0, 1.0, 0.0};
    vec3 center = {0.0, 0.0, 0.0};
    vec3_dup(c->pos, pos);
    vec3_dup(c->center, center);
    vec3_dup(c->up, up);
    camera_update(c);
}

void camera_update (struct camera *c)
{
    mat4x4_look_at (c->view, c->pos, c->center, c->up);
}
