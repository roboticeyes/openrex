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
#include <stdio.h>
#include <stdlib.h>

#include "shader.h"
#include "util.h"

struct shader *shader_load (const char *vs, const char *fs)
{
    char *vertex_source = read_file_ascii (vs);
    if (!vertex_source)
    {
        warn ("Failed to open file %s\n", vs);
        return NULL;
    }

    char *fragment_source = read_file_ascii (fs);
    if (!fragment_source)
    {
        warn ("Failed to open file %s\n", fs);
        return NULL;
    }

    GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertex_shader, 1, (const char *const *) &vertex_source, NULL);
    glCompileShader (vertex_shader);

    GLuint fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fragment_shader, 1, (const char *const *) &fragment_source, NULL);
    glCompileShader (fragment_shader);

    GLint status1, status2;
    glGetShaderiv (vertex_shader, GL_COMPILE_STATUS, &status1);
    if (status1 != GL_TRUE)
    {
        char buffer[512];
        glGetShaderInfoLog (vertex_shader, 512, NULL, buffer);
        warn ("VertexShader error: %s", buffer);
    }

    glGetShaderiv (fragment_shader, GL_COMPILE_STATUS, &status2);
    if (status2 != GL_TRUE)
    {
        char buffer[512];
        glGetShaderInfoLog (fragment_shader, 512, NULL, buffer);
        warn ("FragmentShader error: %s", buffer);
    }

    if (status1 != GL_TRUE || status2 != GL_TRUE)
        return NULL;

    struct shader *s = malloc (sizeof (struct shader));

    s->program = glCreateProgram();

    glAttachShader (s->program, vertex_shader);
    glAttachShader (s->program, fragment_shader);
    glLinkProgram (s->program);

    shader_use (s);

    glDetachShader (s->program, vertex_shader);
    glDetachShader (s->program, fragment_shader);
    glDeleteShader (vertex_shader);
    glDeleteShader (fragment_shader);
    FREE (vertex_source);
    FREE (fragment_source);

    // Load uniforms
    s->projection = glGetUniformLocation (s->program, "projection");
    s->view = glGetUniformLocation (s->program, "view");
    s->model = glGetUniformLocation (s->program, "model");
    s->lightPos = glGetUniformLocation (s->program, "lightPos");
    return s;
}

void shader_use (struct shader *s)
{
    glUseProgram (s->program);
}
