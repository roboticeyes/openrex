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
    GLuint modelview;
};

struct shader *shader_load (const char *vs, const char *fs);
void shader_use (struct shader *s);
