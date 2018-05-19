// OpenGL/ glew Headers
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <string.h>

#include "config.h"
#include "core.h"
#include "global.h"
#include "mesh.h"
#include "shader.h"
#include "status.h"
#include "util.h"

#define WIDTH 1918
#define HEIGHT 2136
#define FOV 45 * 0.0174533f

SDL_Window *win = NULL;
SDL_GLContext *ctx = NULL;

struct mesh m;
struct shader *s;

mat4x4 projection;
GLuint MVP;

void usage (const char *exec)
{
    die ("usage: %s filename.rex\n", exec);
}

enum rex_block_enums
{
    LineSet = 0, Text = 1, Vertex = 2, Mesh = 3, Image = 4, MaterialStandard = 5, PeopleSimulation = 6, UnityPackage = 7
};

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

    s = shader_load ("../viewer/shader.vert", "../viewer/shader.frag");

    mat4x4_perspective (projection, FOV, (float) WIDTH / (float) HEIGHT, 0.001f, 1000.0);

    return 0;
}

void render()
{
    int loop = 1;
    int mouse_x = WIDTH / 2, mouse_y = HEIGHT / 2;
    bool mouse_moved = false;
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
                    default:
                        break;
                }
            }
            /* else if (!mouse_moved && event.type == SDL_MOUSEMOTION) */
            /*     mouse_moved = true; */
        }

        /* SDL_GetMouseState (&mouse_x, &mouse_y); */
        /* printf ("Mouse %d %d\n", mouse_x, mouse_y); */

        /* float deltax = mouse_x - WIDTH / 2; */
        /* float deltay = mouse_y - HEIGHT / 2; */

        /* if ( (deltax != 0 || deltay != 0) && mouseGrab) */
        /* { */
        /*     horizontalAngle += (float) (WIDTH / 2 - mouse_x) * SENSITIVITY; */
        /*     verticalAngle += (float) (HEIGHT / 2 - mouse_y) * SENSITIVITY; */
        /*  */
        /*     if (verticalAngle > C3D_PI / 2.0f) */
        /*         verticalAngle = C3D_PI / 2.0f; */
        /*     else if (verticalAngle < -C3D_PI / 2.0f) */
        /*         verticalAngle = -C3D_PI / 2.0f; */
        /*  */
        /*     SDL_WarpMouseInWindow (window, WIDTH / 2, HEIGHT / 2); */
        /* } */


        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh_render (&m, s, projection);

        SDL_GL_SwapWindow (win);
    }
}

void cleanup()
{
    mesh_free (&m);
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

    mesh_init (&m);
    m.data = mesh;
    mesh_load_vao (&m);
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

    if (argc < 2)
        usage (argv[0]);

    init();
    if (loadrex (argv[1]))
        return 1;

    render();
    cleanup();
    return 0;
}
