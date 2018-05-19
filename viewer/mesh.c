#include "mesh.h"

void mesh_init (struct mesh *m)
{
    m->data = NULL;

    glGenVertexArrays (1, & (m->vao));
    glGenBuffers (1, & (m->vbo));
    glGenBuffers (1, & (m->ibo));
}

void mesh_free (struct mesh *m)
{
    if (!m) return;

    glDeleteVertexArrays (1, & (m->vao));
    glDeleteBuffers (1, & (m->vbo));
    glDeleteBuffers (1, & (m->ibo));
}

void mesh_load_vao (struct mesh *m)
{
    if (!m || !m->data) return;

    glBindVertexArray (m->vao);

    glBindBuffer (GL_ARRAY_BUFFER, m->vbo);
    glBufferData (GL_ARRAY_BUFFER, m->data->nr_vertices * sizeof (GLfloat) * 3, m->data->positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray (0);

    // positions
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // indices
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m->ibo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, m->data->nr_triangles * 3 * sizeof (GLuint), m->data->triangles, GL_STATIC_DRAW);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
}

void mesh_render (struct mesh *m, struct shader *s, mat4x4 projection)
{
    if (!m) return;

    shader_use (s);

    mat4x4 model_view;
    vec3 eye = {2.0, 2.0, 2.0};
    vec3 center = {0.0, 0.0, 0.0};
    vec3 up = {0.0, 1.0, 0.0};
    mat4x4_look_at (model_view, eye, center, up);

    mat4x4 mvp;
    mat4x4_mul (mvp, projection, model_view);
    glUniformMatrix4fv (s->mvp, 1, GL_FALSE, (GLfloat *) mvp);

    glBindVertexArray (m->vao);
    glDrawElements (GL_TRIANGLES, m->data->nr_triangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray (0);
}
