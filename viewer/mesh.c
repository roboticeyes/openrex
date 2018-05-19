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

void mesh_render (struct mesh *m, struct shader *s, struct camera* cam, mat4x4 projection)
{
    if (!m || !cam) return;

    shader_use (s);

    // TODO missing model matrix!

    glUniformMatrix4fv (s->projection, 1, GL_FALSE, (GLfloat *) projection);
    glUniformMatrix4fv (s->modelview, 1, GL_FALSE, (GLfloat *) cam->view);

    glBindVertexArray (m->vao);

    glDrawElements (GL_TRIANGLES, m->data->nr_triangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray (0);
}
