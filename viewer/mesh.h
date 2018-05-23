#pragma once

#include <GL/glew.h>

#include "bbox.h"
#include "camera.h"
#include "core.h"
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
