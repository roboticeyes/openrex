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

#include "global.h"
#include "linmath.h"
#include "polyline.h"
#include "util.h"


void polyline_init (struct polyline *p)
{
    if (!p) return;

    mat4x4_identity (p->model);
    mat4x4_identity (p->transform);

    glGenVertexArrays (1, & (p->vao));
    glGenBuffers (1, & (p->vbo));
}

void polyline_free (struct polyline *p)
{
    if (!p) return;

    glDeleteVertexArrays (1, & (p->vao));
    glDeleteBuffers (1, & (p->vbo));
}

static void polyline_load_vao (struct polyline *p, GLuint elem_size, GLfloat *vertices)
{
    glBindVertexArray (p->vao);

    glBindBuffer (GL_ARRAY_BUFFER, p->vbo);
    /* allocate memory for vertex data */
    glBufferData (GL_ARRAY_BUFFER, p->nr_vertices * sizeof (GLfloat) * elem_size, vertices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray (0);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
}

void polyline_render (struct polyline *p, struct shader *s, struct camera *cam, mat4x4 projection)
{
    if (!p || !cam) return;

    shader_use (s);

    glUniform3f (s->rgb, p->color[0], p->color[1], p->color[2]);
    glUniformMatrix4fv (s->projection, 1, GL_FALSE, (GLfloat *) projection);
    glUniformMatrix4fv (s->view, 1, GL_FALSE, (GLfloat *) cam->view);
    glUniformMatrix4fv (s->model, 1, GL_FALSE, (GLfloat *) p->transform);

    glBindVertexArray (p->vao);

    glDrawArrays (GL_LINE_STRIP, 0, p->nr_vertices);
    glBindVertexArray (0);
}

void polyline_set_rex_lineset (struct polyline *p, struct rex_lineset *data)
{
    if (!p || !data) return;

    p->nr_vertices = data->nr_vertices;

    p->color[0] = data->red;
    p->color[1] = data->green;
    p->color[2] = data->blue;

    GLuint elem_size = 3;
    size_t mem = sizeof (GLfloat) * p->nr_vertices * elem_size;
    GLfloat *vertices = malloc (mem);
    memset (vertices, 0, mem);

    GLfloat *ptr = vertices;
    memcpy (ptr, data->vertices, 12 * data->nr_vertices);
    polyline_load_vao (p, elem_size, vertices);

    FREE (vertices);
}
