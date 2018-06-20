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

#include <GL/glew.h>

#include "bbox.h"
#include "camera.h"
#include "rex-block-pointlist.h"
#include "linmath.h"
#include "shader.h"

/**
 * This is the data for the point list
 */
struct points
{
    GLuint vao;
    GLuint vbo; // vertex data
    GLuint nr_vertices;
    struct bbox bb;
    mat4x4 model; // model matrix
    mat4x4 transform;
};

void points_init (struct points *);
void points_free (struct points *);

void points_center (struct points *p, int center_height);

void points_render (struct points *, struct shader *, struct camera *, mat4x4 projection);

/**
 * Set the actual point data and prepares the point list for rendering
 */
void points_set_rex_pointlist (struct points *p, struct rex_pointlist *data);
