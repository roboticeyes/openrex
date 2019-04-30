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
#pragma once

#include <GL/glew.h>
#include <stdbool.h>

#include "linmath.h"

/**
    This function will load a shader from the disk, compile, attach and then link it.

    @param vs vertex shader file
    @param fs fragment shader file
    @return allocated shader struct (NULL if not successful)
*/

struct shader
{
    GLuint program;
    GLuint projection;
    GLuint view;
    GLuint model;
    GLuint lightPos;
    GLuint diffuse;
    GLuint rgb;
};

struct shader *shader_load (const char *resource_path, const char *vs, const char *fs);
void shader_use (struct shader *s);
