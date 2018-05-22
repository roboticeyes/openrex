#pragma once

#include <GL/glew.h>

#include "bbox.h"
#include "camera.h"
#include "core.h"
#include "linmath.h"
#include "shader.h"

struct mesh
{
    GLuint vao;
    GLuint vbo; // positions
    GLuint ibo; // indices
    struct rex_mesh *data;
    struct bbox bb;
    mat4x4 model; // model matrix
};

void mesh_init (struct mesh *);
void mesh_free (struct mesh *);

void mesh_load_vao (struct mesh *);
void mesh_render (struct mesh *, struct shader *, mat4x4 model, struct camera *, mat4x4 projection);

/**
 * Sets the model matrix to center the model to 0/0/0 using the bounding box
 */
void mesh_center (struct mesh *);

/**
 * Calculate smooth normals of a mesh
 */
void mesh_calc_normals (struct mesh *m);

/**
 * Set the actual mesh data and prepares the mesh for the next steps.
 * Preparation includes bbox calculations, ...
 */
void mesh_set_data(struct mesh *m, struct rex_mesh *data);
