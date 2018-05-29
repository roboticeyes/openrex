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
#include "rex-block-mesh.h"
#include "linmath.h"
#include "shader.h"

/**
 * This is the mesh which can be rendered
 */
struct mesh
{
    GLuint vao;
    GLuint vbo; // vertex data
    GLuint ibo; // indexed triangle data
    GLuint nr_vertices;
    GLuint nr_triangles;
    struct bbox bb;
    mat4x4 model; // model matrix
};

void mesh_init (struct mesh *);
void mesh_free (struct mesh *);

void mesh_render (struct mesh *, struct shader *, mat4x4 model, struct camera *, mat4x4 projection);

/**
 * Set the actual mesh data and prepares the mesh for rendering
 */
void mesh_set_rex_mesh (struct mesh *m, struct rex_mesh *data);
