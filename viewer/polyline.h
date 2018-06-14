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

#include "camera.h"
#include "rex-block-lineset.h"
#include "linmath.h"
#include "shader.h"

/**
 * This is the data for a polyline
 */
struct polyline
{
    GLuint vao;
    GLuint vbo; // vertex data
    GLuint nr_vertices;
    mat4x4 model; // model matrix
    mat4x4 transform;
    vec3 color;
};

void polyline_init (struct polyline *);
void polyline_free (struct polyline *);

void polyline_render (struct polyline *, struct shader *, struct camera *, mat4x4 projection);

void polyline_set_rex_lineset (struct polyline *p, struct rex_lineset *data);
