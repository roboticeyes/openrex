// OpenGL/ glew Headers
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string.h>

#include "config.h"
#include "core.h"
#include "global.h"
#include "status.h"
#include "util.h"

#define WIDTH 800
#define HEIGHT 600

SDL_Window *win = NULL;
SDL_GLContext *ctx = NULL;

/* shader */
GLuint shader_program;
GLuint vertex_shader, fragment_shader;

void usage (const char *exec)
{
    die ("usage: %s filename.rex\n", exec);
}

enum rex_block_enums
{
    LineSet = 0, Text = 1, Vertex = 2, Mesh = 3, Image = 4, MaterialStandard = 5, PeopleSimulation = 6, UnityPackage = 7
};

void print_shader_compile_error (uint32_t id)
{
    int infolen = 0;
    int maxlength = infolen;

    glGetProgramiv (id, GL_INFO_LOG_LENGTH, &maxlength);
    char errorlog[maxlength];

    glGetProgramInfoLog (id, maxlength, &infolen, errorlog);
    printf ("Shader compile error: %s\n", errorlog);
}

/* ------------------------------------------- SHADER -------------------------------------------- */

GLuint LoadShaders (const char *vertex_file_path, const char *fragment_file_path)
{
    // Create the shaders
    GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    char *vs_code = read_file (vertex_file_path);
    if (!vs_code)
    {
        printf ("Cannot open vertex shader\n");
        return;
    }

    // Read the Fragment Shader code from the file
    char *fs_code = read_file (fragment_file_path);
    if (!fs_code)
    {
        printf ("Cannot open fragment shader\n");
        return;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf ("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = vs_code;
    glShaderSource (vertex_shader, 1, &VertexSourcePointer, NULL);
    glCompileShader (vertex_shader);

    // Check Vertex Shader
    glGetShaderiv (vertex_shader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv (vertex_shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char VertexShaderErrorMessage[InfoLogLength + 1];
        glGetShaderInfoLog (vertex_shader, InfoLogLength, NULL, &VertexShaderErrorMessage);
        printf ("%s\n", VertexShaderErrorMessage);
    }

    // Compile Fragment Shader
    printf ("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = fs_code;
    glShaderSource (fragment_shader, 1, &FragmentSourcePointer, NULL);
    glCompileShader (fragment_shader);

    // Check Fragment Shader
    glGetShaderiv (fragment_shader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv (fragment_shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char FragmentShaderErrorMessage [InfoLogLength + 1];
        glGetShaderInfoLog (fragment_shader, InfoLogLength, NULL, &FragmentShaderErrorMessage);
        printf ("%s\n", FragmentShaderErrorMessage);
    }

    // Link the program
    printf ("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader (ProgramID, vertex_shader);
    glAttachShader (ProgramID, fragment_shader);
    glLinkProgram (ProgramID);

    // Check the program
    glGetProgramiv (ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv (ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        /* std::vector<char> ProgramErrorMessage (InfoLogLength + 1); */
        /* glGetProgramInfoLog (ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]); */
        /* printf ("%s\n", &ProgramErrorMessage[0]); */
    }

    glDetachShader (ProgramID, vertex_shader);
    glDetachShader (ProgramID, fragment_shader);

    glDeleteShader (vertex_shader);
    glDeleteShader (fragment_shader);

    return ProgramID;
}


void init_shader()
{
    shader_program = glCreateProgram();

    glBindAttribLocation (shader_program, 0, "in_Position");
    glBindAttribLocation (shader_program, 1, "in_Color");

    // load vertex shader
    /* char *vs_file = read_file ("view.vert"); */
    /* if (!vs_file) */
    /* { */
    /*     printf ("Cannot open vertex shader\n"); */
    /*     return; */
    /* } */

    const GLchar *vs_file[] = { "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }" };

    vertex_shader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertex_shader, 1, vs_file, NULL);
    glCompileShader (vertex_shader);

    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv (vertex_shader, GL_COMPILE_STATUS, &vShaderCompiled);
    if (vShaderCompiled != GL_TRUE)
    {
        printf ("Unable to compile vertex shader %d!\n", vertex_shader);
        print_shader_compile_error (vertex_shader);
        return;
    }

    /* int compiled = 0; */
    /* glGetShaderiv (vertex_shader, GL_COMPILE_STATUS, &compiled); */
    /*  */
    /* if (compiled == 0) */
    /* { */
    /*     print_shader_compile_error (vertex_shader); */
    /*     return; */
    /* } */
    glAttachShader (shader_program, vertex_shader);


    // load fragment shader
    char *fs_file = read_file ("view.frag");
    if (!fs_file)
    {
        printf ("Cannot open fragment shader\n");
        return;
    }
    const int fs_len = strlen (fs_file);
    fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fragment_shader, 1, &fs_file, &fs_len);
    glCompileShader (fragment_shader);
    glGetShaderiv (fragment_shader, GL_COMPILE_STATUS, &vShaderCompiled);
    if (!vShaderCompiled)
    {
        printf ("error compiling fragment shader\n");
        return;
    }
    glAttachShader (shader_program, fragment_shader);

    // link shader
    glLinkProgram (shader_program);
    int status;
    glGetProgramiv (shader_program, GL_LINK_STATUS, &status);
    if (!status)
        printf ("Error linking shader\n");

    free (vs_file);
    free (fs_file);
}

void use_shader()
{

}

/* ----------------------------------------- SHADER END ------------------------------------------ */

int init()
{
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

    win = SDL_CreateWindow ("rex-view",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            WIDTH, HEIGHT,
                            SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    if (win == NULL)
    {
        printf ("Cannot create window: %s\n", SDL_GetError());
        return 1;
    }

    ctx = SDL_GL_CreateContext (win);

    if (ctx == NULL)
    {
        printf ("Cannot create OpenGL context: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval (1);

#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow (win);

    LoadShaders ("../tools/view.vert", "../tools/view.frag");
    /* init_shader(); */

    return 0;
}

void render()
{
    int loop = 1;
    while (loop)
    {
        SDL_Event event;
        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_QUIT)
                loop = 0;

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        loop = 0;
                        break;
                    case SDLK_r:
                        glClearColor (1.0, 0.0, 0.0, 1.0);
                        glClear (GL_COLOR_BUFFER_BIT);
                        SDL_GL_SwapWindow (win);
                        break;
                    case SDLK_g:
                        glClearColor (0.0, 1.0, 0.0, 1.0);
                        glClear (GL_COLOR_BUFFER_BIT);
                        SDL_GL_SwapWindow (win);
                        break;
                    case SDLK_b:
                        glClearColor (0.0, 0.0, 1.0, 1.0);
                        glClear (GL_COLOR_BUFFER_BIT);
                        SDL_GL_SwapWindow (win);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void cleanup()
{
    SDL_GL_DeleteContext (ctx);
    SDL_DestroyWindow (win);
    SDL_Quit();
}

void loadmesh (struct rex_mesh *mesh)
{
    if (!mesh)
        return;

    printf ("name                   %20s\n", (mesh->name) ? mesh->name : "");
    printf ("vertices               %20u\n", mesh->nr_vertices);
    printf ("triangles              %20u\n", mesh->nr_triangles);
    printf ("normals                %20s\n", (mesh->normals) ? "yes" : "no");
    printf ("texture coords         %20s\n", (mesh->tex_coords) ? "yes" : "no");
    printf ("vertex colors          %20s\n", (mesh->colors) ? "yes" : "no");

    // VBOs
    const uint32_t nr_vbos = 1; // pos only
    GLuint vbo[nr_vbos];
    glGenBuffers (nr_vbos, vbo);

    glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
    uint32_t sz = mesh->nr_vertices * 3 * sizeof (GLfloat);
    printf ("sz [bytes]: %d\n", sz);
    fflush (stdout);
    glBufferData (
        GL_ARRAY_BUFFER,
        sz,
        mesh->positions,
        GL_STATIC_DRAW);

    // VAO
    GLuint vao[1]; // only one per mesh
    glGenVertexArrays (1, vao);
    glBindVertexArray (vao[0]);

    const uint32_t pos_idx = 0;
    glVertexAttribPointer (
        pos_idx,
        3, // x/y/z
        GL_FLOAT,
        GL_FALSE, // normalize
        0, // stride
        0 // data ptr
    );

    glEnableVertexAttribArray (pos_idx);

    /* float *p = mesh->positions; */
    /* for (int i = 0; i < mesh->nr_vertices * 3; i += 3) */
    /*     printf ("v %f %f %f\n", p[i], p[i + 1], p[i + 2]); */
    /* uint32_t *t = mesh->triangles; */
    /* for (int i = 0; i < mesh->nr_triangles * 3; i += 3) */
    /*     printf ("f %d %d %d\n", t[i] + 1, t[i + 1] + 1, t[i + 2] + 1); */
}

int loadrex (const char *file)
{
    struct rex_header header;
    FILE *fp = fopen (file, "rb");

    if (!fp)
    {
        printf ("Cannot open REX file %s\n", file);
        return 1;
    }

    rex_read_header (fp, &header);

    for (int i = 0; i < header.nr_datablocks; i++)
    {
        struct rex_block_header block_header;
        rex_read_data_block_header (fp, &block_header);

        if (block_header.type == Mesh)
        {
            struct rex_mesh_header header;
            struct rex_mesh mesh = { 0 };
            mesh.id = block_header.id;
            rex_read_mesh_block (fp, block_header.sz, &header, &mesh);
            /* load rex mesh */
            loadmesh (&mesh);
            rex_mesh_free (&mesh);
        }
        else fseek (fp, block_header.sz, SEEK_CUR);
    }
    fclose (fp);
    return 0;
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    /* if (argc < 2) */
    /*     usage (argv[0]); */

    init();

    /* if (loadrex (argv[1])) */
    if (loadrex ("../data/oscar.rex"))
        return 1;

    render();

    cleanup();

    return 0;
}
