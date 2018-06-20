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

#include "bbox.h"
#include "linmath.h"
#include "mesh.h"
#include "shader.h"

/**
 * A mesh group contains a list of meshes which can be rendered. The group
 * itself has its own transformation matrix and contains the list of meshes
 */
struct mesh_group
{
    struct bbox bb;
    mat4x4 transform;
    struct list *meshes;
};

void mesh_group_init (struct mesh_group *g);
void mesh_group_destroy (struct mesh_group *g);

void mesh_group_add_mesh (struct mesh_group *g, struct mesh *m);
void mesh_group_remove_mesh (struct mesh_group *g, struct mesh *m);

void mesh_group_calc_bbox (struct mesh_group *g);
void mesh_group_center (struct mesh_group *g, int center_height);

void mesh_group_render (struct mesh_group *g, struct shader *s, struct camera *cam, mat4x4 projection);
