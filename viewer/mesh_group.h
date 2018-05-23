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
void mesh_group_center (struct mesh_group *g);

void mesh_group_render (struct mesh_group *g, struct shader *s, struct camera *cam, mat4x4 projection);
