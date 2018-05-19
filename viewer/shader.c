#include <stdio.h>
#include <stdlib.h>

#include "shader.h"
#include "util.h"

struct shader *shader_load (const char *vs, const char *fs)
{
    char *vertex_source = read_file (vs);
    if (!vertex_source)
    {
        warn ("Failed to open file %s\n", vs);
        return NULL;
    }

    char *fragment_source = read_file (fs);
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
    free (vertex_source);
    free (fragment_source);

    // Load uniforms
	s->mvp = glGetUniformLocation(s->program, "MVP");
    return s;
}

void shader_use (struct shader *s)
{
    glUseProgram (s->program);
}
