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

/**
 * \file
 * \brief This is the scene which gets rendered
 *
 * The scene contains all geometry, light, and camera information.
 */

#include "rex.h"
#include "mesh.h"
#include "mesh_group.h"
#include "points.h"
#include "polyline.h"

enum render_mode
{
    SOLID,
    WIREFRAME
};

/**
 * This is the scene definition which contains all relevant objects for rendering
 */
struct scene
{
    struct mesh_group meshes;
    struct points pointcloud;
    struct camera cam;
};

struct scene *scene_create (const char *);

void scene_destroy (struct scene *);

void scene_update (struct scene *);

void scene_render (struct scene *s, mat4x4 projection);

void scene_add_mesh (struct scene *s, struct mesh *m);

void scene_set_render_mode (enum render_mode m);

void scene_activate_pointcloud (struct scene *s);

/**
 * Centers the complete scene ignoring the height information
 */
void scene_center(struct scene *s);

