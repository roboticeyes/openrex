#pragma once

#include "linmath.h"

struct camera
{
    vec3 pos;
    vec3 center;
    vec3 up;     // up vector of the camera
    float orbit; // orbit angle around the object (degrees)
    float pitch; // height angle of the camera (degrees)
    float distance; // distance to object
    mat4x4 view;
};

void camera_init (struct camera *c, vec3 pos);
void camera_update (struct camera *C);
