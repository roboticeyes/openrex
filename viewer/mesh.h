#pragma once

#include <GL/glew.h>

#include "camera.h"
#include "core.h"
#include "shader.h"
#include "linmath.h"

struct mesh
{
    GLuint vao;
    GLuint vbo; // positions
    GLuint ibo; // indices
    struct rex_mesh *data;
};

void mesh_init (struct mesh *);
void mesh_free (struct mesh *);

void mesh_load_vao (struct mesh *);
void mesh_render (struct mesh *, struct shader *, struct camera *, mat4x4 projection);

void mesh_calc_normals(struct mesh *m);
