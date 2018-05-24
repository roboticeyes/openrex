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
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof (GLfloat) * 3 * m->nr_vertices));
    glEnableVertexAttribArray (1);

    // colors
    glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof (GLfloat) * 6 * m->nr_vertices));
    glEnableVertexAttribArray (2);

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

/**
 * Ptr is pointing to a valid memory which will be used to store normal information
 */
static void mesh_calc_normals (float *ptr, struct rex_mesh *rm)
{
    if (!ptr || !rm) return;

    if (rm->normals)
        memcpy (ptr, rm->normals, 12 * rm->nr_vertices);
    else
        memset (ptr, 0, 12 * rm->nr_vertices);

#if 0
    //  currently no normal are calculated
    vec3 v0, v1, v2;
    vec3 a, b;
    vec3 r;
    for (int i = 0; i < rm->nr_triangles * 3; i += 3, ptr += 3)
    {
        get_vertex (&v0, rm->positions, rm->triangles[i]);
        get_vertex (&v1, rm->positions, rm->triangles[i + 1]);
        get_vertex (&v2, rm->positions, rm->triangles[i + 2]);

        vec3_sub (a, v1, v0);
        vec3_sub (b, v2, v0);
        vec3_mul_cross (r, a, b);
        vec3_norm (ptr, r);
        /* printf ("norm: %f %f %f\n", n[0], n[1], n[2]); */
        /* fflush (stdout); */
    }
#endif
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

    GLuint elem_size = 9; // pos, normals, color
    size_t mem = sizeof (GLfloat) * m->nr_vertices * elem_size;
    GLfloat *vertices = malloc (mem);
    memset (vertices, 0, mem);

    GLfloat *ptr = vertices;
    memcpy (ptr, data->positions, 12 * data->nr_vertices);
    ptr = &vertices[3 * data->nr_vertices];

    mesh_calc_normals (ptr, data);
    ptr = &vertices[6 * data->nr_vertices];

    if (data->colors)
        memcpy (ptr, data->colors, 12 * data->nr_vertices);
    else
    {
        // TODO set mesh material
        for (int i = 0; i < 3 * data->nr_vertices; i++, ptr++)
            (*ptr) = 0.5f;
    }

    // triangles
    uint32_t *indices = malloc (sizeof (uint32_t) * m->nr_triangles * 3);
    indices = memcpy (indices, data->triangles, 12 * data->nr_triangles);

    mesh_load_vao (m, elem_size, vertices, indices);

    FREE (vertices);
    FREE (indices);
}
