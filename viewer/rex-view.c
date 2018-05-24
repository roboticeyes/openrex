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
#include "mesh_group.h"
#include "shader.h"
#include "status.h"
#include "util.h"

#define NEAR 0.1
#define FAR 100.0
#define WIDTH 1918
#define HEIGHT 2136
#define FOV (45 * 0.0174533f)

SDL_Window *win = NULL;
SDL_GLContext *ctx = NULL;

struct mesh_group root;
struct camera cam;
mat4x4 projection;
struct shader *s;

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
    mesh_group_init (&root);

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

    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glCullFace (GL_BACK);

    SDL_GL_SwapWindow (win);

    s = shader_load ("../viewer/shader.vert", "../viewer/shader.frag");

    mat4x4_perspective (projection, FOV, (float) WIDTH / (float) HEIGHT, NEAR, FAR);
    vec3 initial_pos = {0.0, 0.0, 10.0};
    camera_init (&cam, initial_pos);

    return 0;
}

static void set_zoom (int value)
{
    cam.distance += value;
    if (cam.distance < 1) cam.distance = 1;
    if (cam.distance > 100) cam.distance = 100;
    camera_update (&cam);
}

void render()
{
    int loop = 1;
    bool wireframe = false;
    bool mouse_moved = false;
    bool mouse_pressed = false;
    vec2 mouse_pos = {0.0, 0.0};

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
                    case SDLK_w:
                        wireframe = !wireframe;
                        break;
                    case SDLK_h:
                        set_zoom (-1);
                        break;
                    case SDLK_l:
                        set_zoom (1);
                        break;
                    case SDLK_UP:
                        cam.pitch += 5;
                        camera_update (&cam);
                        break;
                    case SDLK_DOWN:
                        cam.pitch -= 5;
                        camera_update (&cam);
                        break;
                    case SDLK_j:
                    case SDLK_LEFT:
                        cam.orbit += 5;
                        camera_update (&cam);
                        break;
                    case SDLK_k:
                    case SDLK_RIGHT:
                        cam.orbit -= 5;
                        camera_update (&cam);
                        break;
                    default:
                        break;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (!mouse_pressed && event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_pressed = true;
                    mouse_pos[0] = event.button.x;
                    mouse_pos[1] = event.button.y;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (mouse_pressed && event.button.button == SDL_BUTTON_LEFT)
                    mouse_pressed = false;
            }
            else if (!mouse_moved && event.type == SDL_MOUSEMOTION)
                mouse_moved = true;
            else if (event.type == SDL_MOUSEWHEEL)
                set_zoom (-event.wheel.y);
        }

        if (mouse_pressed && mouse_moved)
        {
            int mx, my;
            SDL_GetMouseState (&mx, &my);
            float dx = (mouse_pos[0] - mx) * 0.01;
            float dy = (mouse_pos[1] - my) * 0.01;
            cam.orbit += dx;
            cam.pitch -= dy;
            camera_update (&cam);
        }

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update camera position for light position
        glUniform3fv (s->lightPos, 1, (GLfloat *) cam.pos);

        if (wireframe)
            glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        mesh_group_render (&root, s, &cam, projection);
        if (wireframe)
            glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

        SDL_GL_SwapWindow (win);
        SDL_Delay (1);
    }
}

void cleanup()
{
    mesh_group_destroy (&root);
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

    struct mesh *m;
    m = malloc (sizeof (struct mesh));
    mesh_init (m);
    mesh_set_rex_mesh (m, mesh);
    mesh_group_add_mesh (&root, m);
    rex_mesh_free (mesh);
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

    int meshes = 0;
    for (int i = 0; i < header.nr_datablocks; i++)
    {
        struct rex_block_header block_header;
        rex_read_data_block_header (fp, &block_header);

        if (block_header.type == Mesh)
        {
            struct rex_mesh_header header;
            struct rex_mesh *mesh = malloc (sizeof (struct rex_mesh));
            mesh->id = block_header.id;
            rex_read_mesh_block (fp, block_header.sz, &header, mesh);
            loadmesh (mesh);
            meshes++;
        }
        else fseek (fp, block_header.sz, SEEK_CUR);
    }
    fclose (fp);

    if (!meshes)
    {
        printf ("Nothing found to render, go home!\n");
        return 1;
    }

    mesh_group_center (&root);
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

    printf("Scene completely loaded, start rendering ...\n"); fflush(stdout);
    render();
    cleanup();
    return 0;
}
