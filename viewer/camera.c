#include <math.h>

#include "camera.h"
#include "linmath.h"
#include <stdio.h>

void camera_init (struct camera *c, vec3 pos)
{
    if (!c) return;
    vec3 up = {0.0, 1.0, 0.0};
    vec3 center = {0.0, 0.0, 0.0};
    vec3_dup (c->pos, pos);
    vec3_dup (c->center, center);
    vec3_dup (c->up, up);
    c->orbit = 0;
    c->pitch = 30;
    c->distance = 10;
    camera_update (c);
}

static float calc_dist_horizontal (float dist, float pitch)
{
    return dist * cosf (deg2rad (pitch));
}

static float calc_dist_vertical (float dist, float pitch)
{
    return dist * sinf (deg2rad (pitch));
}

static void cam_pos_update (struct camera *c)
{
    if (!c) return;

    // check boundaries
    const float MAX_PITCH = 89.9f;
    if (c->pitch < -MAX_PITCH) c->pitch = -MAX_PITCH;
    if (c->pitch > MAX_PITCH) c->pitch = MAX_PITCH;
    float ofs_x = calc_dist_horizontal(c->distance, c->pitch) * sinf (deg2rad (c->orbit));
    float ofs_z = calc_dist_horizontal(c->distance, c->pitch) * cosf (deg2rad (c->orbit));
    c->pos[0] = ofs_x;
    c->pos[1] = calc_dist_vertical (c->distance, c->pitch);
    c->pos[2] = ofs_z;
}

void camera_update (struct camera *c)
{
    cam_pos_update (c);
    /* printf("update camera %f %f %f\n", c->pos[0], c->pos[1], c->pos[2]); fflush(stdout); */
    mat4x4_look_at (c->view, c->pos, c->center, c->up);
}
