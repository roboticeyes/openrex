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

#include "scene.h"
#include "axis.h"

// Shaders are currently fixed
struct shader *shader_mesh;
struct shader *shader_pointcloud;
struct shader *shader_lines;

enum render_mode mode = SOLID;

int has_mesh = 0;
int has_pointcloud = 0;

struct scene *scene_create (const char *resource_path)
{
    struct scene *s = malloc (sizeof (struct scene));

    // Initialize camera
    vec3 initial_pos = {0.0, 0.0, 10.0};
    camera_init (&s->cam, initial_pos);

    // Initialize meshes
    mesh_group_init (&s->meshes);

    // Initialize pointcloud
    points_init (&s->pointcloud);

    // Initialize/load shaders
    shader_mesh       = shader_load (resource_path, "mesh.vs", "mesh.fs");
    shader_pointcloud = shader_load (resource_path, "pointcloud.vs", "pointcloud.fs");
    shader_lines      = shader_load (resource_path, "polyline.vs", "polyline.fs");

    axis_init();

    return s;
}

void scene_destroy (struct scene *s)
{
    mesh_group_destroy (&s->meshes);
    points_free (&s->pointcloud);
    axis_destroy();
}

void scene_update (struct scene *s)
{

}

void scene_render (struct scene *s, mat4x4 projection)
{
    // Axis
    axis_render (shader_lines, &s->cam, projection);

    if (mode == WIREFRAME)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    if (has_mesh)
    {
        glUniform3fv (shader_mesh->lightPos, 1, (GLfloat *) s->cam.pos);
        mesh_group_render (&s->meshes, shader_mesh, &s->cam, projection);
    }

    if (has_pointcloud)
    {
        glUniform3fv (shader_pointcloud->lightPos, 1, (GLfloat *) s->cam.pos);
        points_render (&s->pointcloud, shader_pointcloud, &s->cam, projection);
    }

    if (mode == WIREFRAME)
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void scene_add_mesh (struct scene *s, struct mesh *m)
{
    mesh_group_add_mesh (&s->meshes, m);
    has_mesh = 1;
}

void scene_center(struct scene *s)
{
    int center_height = 0; // do not center the height value (z)
    mesh_group_center (&s->meshes, center_height);
    points_center (&s->pointcloud, center_height);
}

void scene_set_render_mode (enum render_mode m)
{
    mode = m;
}

void scene_activate_pointcloud (struct scene *s)
{
    has_pointcloud = 1;
}
