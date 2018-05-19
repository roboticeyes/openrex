#pragma once

#include <GL/glew.h>
#include "core.h"
#include "shader.h"

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
void mesh_render (struct mesh *, struct shader *);
