#include "mesh.h"
#include "bbox.h"
#include "linmath.h"

void mesh_init (struct mesh *m)
{
    if (!m) return;
    m->data = NULL;

    bbox_init(&m->bb);
    mat4x4_identity(m->model);

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

void mesh_render (struct mesh *m, struct shader *s, mat4x4 model, struct camera *cam, mat4x4 projection)
{
    if (!m || !cam) return;

    shader_use (s);

    glUniformMatrix4fv (s->projection, 1, GL_FALSE, (GLfloat *) projection);
    glUniformMatrix4fv (s->view, 1, GL_FALSE, (GLfloat *) cam->view);
    glUniformMatrix4fv (s->model, 1, GL_FALSE, (GLfloat *) model);

    glBindVertexArray (m->vao);

    glDrawElements (GL_TRIANGLES, m->data->nr_triangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray (0);
}

inline static void get_vertex(vec3 *pos, float* positions, uint32_t index)
{
    (*pos)[0] = positions[index * 3];
    (*pos)[1] = positions[index * 3+1];
    (*pos)[2] = positions[index * 3+2];
}

void mesh_calc_normals (struct mesh *m)
{
    if (!m || !m->data) return;

    /* rex_mesh_dump_obj (m->data); */

    struct rex_mesh *rmesh = m->data;
    const int MAX_TRI = 10;
    vec3 normals[rmesh->nr_vertices][MAX_TRI];

    for (int i = 0; i < rmesh->nr_triangles * 3; i += 3)
    {
        vec3 v0,v1,v2;
        get_vertex(&v0, rmesh->positions, rmesh->triangles[i]);
        get_vertex(&v1, rmesh->positions, rmesh->triangles[i+1]);
        get_vertex(&v2, rmesh->positions, rmesh->triangles[i+2]);

        vec3 a, b;
        vec3_sub (a, v1, v0);
        vec3_sub (b, v2, v0);
        vec3 r, n;
        vec3_mul_cross (r, a, b);
        vec3_norm (n, r);
        /* printf ("norm: %f %f %f\n", n[0], n[1], n[2]); */

        // store pre-face normal for each vertex
        /* normals[rmesh->triangles[i]] = n; */
        /* normals[rmesh->triangles[i+1]] = n; */
        /* normals[rmesh->triangles[i+2]] = n; */

        fflush (stdout);
    }
}

void mesh_calc_bbox(struct mesh *m)
{
    if (!m || !m->data) return;
    struct rex_mesh *rmesh = m->data;
    bbox_init(&m->bb);
    for (int i=0; i<rmesh->nr_vertices*3; i+=3)
    {
        // x
        if (rmesh->positions[i]>m->bb.max[0]) m->bb.max[0] = rmesh->positions[i];
        if (rmesh->positions[i]<m->bb.min[0]) m->bb.min[0] = rmesh->positions[i];
        // y
        if (rmesh->positions[i+1]>m->bb.max[1]) m->bb.max[1] = rmesh->positions[i+1];
        if (rmesh->positions[i+1]<m->bb.min[1]) m->bb.min[1] = rmesh->positions[i+1];
        // z
        if (rmesh->positions[i+2]>m->bb.max[2]) m->bb.max[2] = rmesh->positions[i+2];
        if (rmesh->positions[i+2]<m->bb.min[2]) m->bb.min[2] = rmesh->positions[i+2];

    }
    printf("Mesh bounding box:\n");
    vec3_dump("  min", m->bb.min);
    vec3_dump("  max", m->bb.max);
}

void mesh_center (struct mesh *m)
{
    if (!m) return;

    float tx = -(m->bb.min[0] + (m->bb.max[0] - m->bb.min[0]));
    float ty = -(m->bb.min[1] + (m->bb.max[1] - m->bb.min[1]));
    float tz = -(m->bb.min[2] + (m->bb.max[2] - m->bb.min[2]));

    mat4x4_translate(m->model, tx, ty, tz);
    mat4x4_dump(m->model);
}

void mesh_set_data(struct mesh *m, struct rex_mesh *data)
{
    if (!m) return;
    m->data = data;
    mesh_calc_bbox(m);
}
