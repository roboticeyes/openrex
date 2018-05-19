#pragma once

#include "linmath.h"

struct camera
{
    vec3 pos;
    vec3 center;
    vec3 up;
    float pitch;
    float yaw;
    float roll;
    mat4x4 view;
};

void camera_init (struct camera *c, vec3 pos);
void camera_update (struct camera *C);
