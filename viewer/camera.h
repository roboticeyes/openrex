/*
 * Copyright 2018 Robotic Eyes GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.*
 */
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
