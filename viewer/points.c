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
#include "points.h"
#include "util.h"

void points_init (struct points *p)
{
    if (!p) return;

    bbox_init (&p->bb);
    mat4x4_identity (p->model);
    mat4x4_identity (p->transform);

    glGenVertexArrays (1, & (p->vao));
    glGenBuffers (1, & (p->vbo));
}

void points_free (struct points *p)
{
    if (!p) return;

    glDeleteVertexArrays (1, & (p->vao));
    glDeleteBuffers (1, & (p->vbo));
}

static void points_load_vao (struct points *p, GLuint elem_size, GLfloat *vertices)
{
    printf("Binding VAO %d\n", p->vao);
    glBindVertexArray (p->vao);

    glBindBuffer (GL_ARRAY_BUFFER, p->vbo);
    /* allocate memory for vertex data */
    glBufferData (GL_ARRAY_BUFFER, p->nr_vertices * sizeof (GLfloat) * elem_size, vertices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray (0);

    // colors
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof (GLfloat) * 3 * p->nr_vertices));
    glEnableVertexAttribArray (1);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
}

void points_render (struct points *p, struct shader *s, struct camera *cam, mat4x4 projection)
{
    if (!p || !cam) return;

    shader_use (s);

    glUniformMatrix4fv (s->projection, 1, GL_FALSE, (GLfloat *) projection);
    glUniformMatrix4fv (s->view, 1, GL_FALSE, (GLfloat *) cam->view);
    glUniformMatrix4fv (s->model, 1, GL_FALSE, (GLfloat *) p->transform);

    glBindVertexArray (p->vao);

    glDrawArrays (GL_POINTS, 0, p->nr_vertices);
    glBindVertexArray (0);
}

static void points_calc_bbox (struct points *p, struct rex_pointlist *rp)
{
    if (!p || !rp) return;
    bbox_init (&p->bb);
    for (int i = 0; i < rp->nr_vertices * 3; i += 3)
    {
        // x
        if (rp->positions[i] > p->bb.max[0]) p->bb.max[0] = rp->positions[i];
        if (rp->positions[i] < p->bb.min[0]) p->bb.min[0] = rp->positions[i];
        // y
        if (rp->positions[i + 1] > p->bb.max[1]) p->bb.max[1] = rp->positions[i + 1];
        if (rp->positions[i + 1] < p->bb.min[1]) p->bb.min[1] = rp->positions[i + 1];
        // z
        if (rp->positions[i + 2] > p->bb.max[2]) p->bb.max[2] = rp->positions[i + 2];
        if (rp->positions[i + 2] < p->bb.min[2]) p->bb.min[2] = rp->positions[i + 2];

    }
    printf ("points bounding box:\n");
    vec3_dump ("  min", p->bb.min);
    vec3_dump ("  max", p->bb.max);
}

void points_center (struct points *p)
{
    if (!p) return;

    float tx = - (p->bb.min[0] + (p->bb.max[0] - p->bb.min[0]) / 2.0);
    float ty = - (p->bb.min[1] + (p->bb.max[1] - p->bb.min[1]) / 2.0);
    float tz = - (p->bb.min[2] + (p->bb.max[2] - p->bb.min[2]) / 2.0);

    mat4x4_translate (p->transform, tx, ty, tz);
    mat4x4_dump (p->transform);
}


void points_set_rex_pointlist (struct points *p, struct rex_pointlist *data)
{
    if (!p || !data) return;

    p->nr_vertices = data->nr_vertices;

    points_calc_bbox (p, data);

    GLuint elem_size = 6; // pos, color
    size_t mem = sizeof (GLfloat) * p->nr_vertices * elem_size;
    GLfloat *vertices = malloc (mem);
    memset (vertices, 0, mem);

    GLfloat *ptr = vertices;
    memcpy (ptr, data->positions, 12 * data->nr_vertices);
    ptr = &vertices[3 * data->nr_vertices];

    if (data->nr_colors)
        memcpy (ptr, data->colors, 12 * data->nr_colors);
    else
    {
        // set a default color
        for (int i = 0; i < 3 * data->nr_vertices; i++, ptr++)
            (*ptr) = 0.8f;
    }

    points_load_vao (p, elem_size, vertices);

    FREE (vertices);
}
