#include <stdlib.h>
#include <string.h>

#include "bbox.h"
#include "global.h"
#include "linmath.h"
#include "mesh.h"
#include "util.h"

void mesh_init (struct mesh *m)
{
    if (!m) return;

    bbox_init (&m->bb);
    mat4x4_identity (m->model);

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

static void mesh_load_vao (struct mesh *m, GLuint elem_size, GLfloat *vertices, GLuint *indices)
{
    glBindVertexArray (m->vao);

    glBindBuffer (GL_ARRAY_BUFFER, m->vbo);
    /* allocate memory for vertex data */
    glBufferData (GL_ARRAY_BUFFER, m->nr_vertices * sizeof (GLfloat) * elem_size, vertices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray (0);

    // normals
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(GLfloat) * 3 * m->nr_vertices));
    glEnableVertexAttribArray (1);

    // indices
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m->ibo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, m->nr_triangles * 3 * sizeof (GLuint), indices, GL_STATIC_DRAW);

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

    glDrawElements (GL_TRIANGLES, m->nr_triangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray (0);
}

inline static void get_vertex (vec3 *pos, float *positions, uint32_t index)
{
    (*pos) [0] = positions[index * 3];
    (*pos) [1] = positions[index * 3 + 1];
    (*pos) [2] = positions[index * 3 + 2];
}

static void mesh_calc_normals (struct mesh *m, struct rex_mesh *rm)
{
    if (!m || !rm) return;

    if (rm->normals)
    {
        warn("Mesh has normals, nothing to calculate");
        return;
    }
    rm->normals = malloc(12 * rm->nr_vertices);
    float *n_ptr = rm->normals;

    /* const int MAX_TRI = 10; */
    /* vec3 normals[rm->nr_vertices][MAX_TRI]; */

    for (int i = 0; i < rm->nr_triangles * 3; i += 3)
    {
        vec3 v0, v1, v2;
        get_vertex (&v0, rm->positions, rm->triangles[i]);
        get_vertex (&v1, rm->positions, rm->triangles[i + 1]);
        get_vertex (&v2, rm->positions, rm->triangles[i + 2]);

        vec3 a, b;
        vec3_sub (a, v1, v0);
        vec3_sub (b, v2, v0);
        vec3 r, n;
        vec3_mul_cross (r, a, b);
        vec3_norm (n, r);
        /* printf ("norm: %f %f %f\n", n[0], n[1], n[2]); */
        /* fflush (stdout); */

        // store normal for all vertices (currently shared vertices are getting replaced)
        *n_ptr = n[0];
        n_ptr++;
        *n_ptr = n[1];
        n_ptr++;
        *n_ptr = n[2];
        n_ptr++;

        // store pre-face normal for each vertex
        /* normals[rm->triangles[i]] = n; */
        /* normals[rm->triangles[i+1]] = n; */
        /* normals[rm->triangles[i+2]] = n; */
    }
}

static void mesh_calc_bbox (struct mesh *m, struct rex_mesh *rm)
{
    if (!m || !rm) return;
    bbox_init (&m->bb);
    for (int i = 0; i < rm->nr_vertices * 3; i += 3)
    {
        // x
        if (rm->positions[i] > m->bb.max[0]) m->bb.max[0] = rm->positions[i];
        if (rm->positions[i] < m->bb.min[0]) m->bb.min[0] = rm->positions[i];
        // y
        if (rm->positions[i + 1] > m->bb.max[1]) m->bb.max[1] = rm->positions[i + 1];
        if (rm->positions[i + 1] < m->bb.min[1]) m->bb.min[1] = rm->positions[i + 1];
        // z
        if (rm->positions[i + 2] > m->bb.max[2]) m->bb.max[2] = rm->positions[i + 2];
        if (rm->positions[i + 2] < m->bb.min[2]) m->bb.min[2] = rm->positions[i + 2];

    }
    printf ("Mesh bounding box:\n");
    vec3_dump ("  min", m->bb.min);
    vec3_dump ("  max", m->bb.max);
}

void mesh_set_rex_mesh (struct mesh *m, struct rex_mesh *data)
{
    if (!m || !data) return;

    m->nr_triangles = data->nr_triangles;
    m->nr_vertices = data->nr_vertices;
    mesh_calc_bbox (m, data);
    mesh_calc_normals(m, data);
    GLuint elem_size = 6; // pos and normals
    GLfloat *vertices = malloc (sizeof (GLfloat) * m->nr_vertices * elem_size);
    memset (vertices, 0, sizeof (GLfloat) * m->nr_vertices * elem_size);

    GLfloat *ptr = vertices;
    memcpy (ptr, data->positions, 12 * data->nr_vertices);
    ptr += 3 * data->nr_vertices;
    if (data->normals)
        memcpy (ptr, data->normals, 12 * data->nr_vertices);

    uint32_t *indices = malloc (sizeof (uint32_t) * m->nr_triangles * 3);
    memcpy (indices, data->triangles, 12 * data->nr_triangles);

    mesh_load_vao (m, elem_size, vertices, indices);

    FREE (vertices);
    FREE (indices);
}
