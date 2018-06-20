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

// OpenGL/ glew Headers
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "gameengine.h"
#include "util.h"

static SDL_Window *win = NULL;
static SDL_GLContext *ctx = NULL;

// Framerate handling
static char frame_rate_string_var[32];
static int frame_rate_var              = 0;
static double frame_time_var           = 0.0;
static unsigned long frame_start_time  = 0.0;
static unsigned long frame_end_time    = 0.0;
static const double frame_update_rate  = 0.5;
static int frame_counter               = 0;
static double frame_acc_time           = 0.0;

static mat4x4 projection_matrix;

// Pointer to the scene to be rendered
struct scene *render_scene;

// TODO currently fixed and does not react to window changes
#define NEAR 0.1
#define FAR 1000.0
#define WIDTH 1918
#define HEIGHT 2136
#define FOV (45 * 0.0174533f)

// forward declarations
static void init();
static void render();
static void frame_begin();
static void frame_end();

void gameengine_init()
{
    init();
}

void gameengine_start (struct scene *s)
{
    render_scene = s;
    render();
}

void gameengine_cleanup()
{
    SDL_GL_DeleteContext (ctx);
    SDL_DestroyWindow (win);
    SDL_Quit();
}

static void set_zoom (int value)
{
    render_scene->cam.distance += value;
    if (render_scene->cam.distance < 1) render_scene->cam.distance = 1;
    if (render_scene->cam.distance > 1000) render_scene->cam.distance = 1000;
    camera_update (&render_scene->cam);
}

static void render()
{
    int loop = 1;
    bool wireframe = false;
    bool mouse_moved = false;
    bool mouse_pressed = false;
    vec2 mouse_pos = {0.0, 0.0};

    while (loop)
    {
        frame_begin();
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
                        if (wireframe)
                            scene_set_render_mode (WIREFRAME);
                        else
                            scene_set_render_mode (SOLID);
                        break;
                    case SDLK_h:
                        set_zoom (-1);
                        break;
                    case SDLK_l:
                        set_zoom (1);
                        break;
                    case SDLK_UP:
                        render_scene->cam.pitch += 5;
                        camera_update (&render_scene->cam);
                        break;
                    case SDLK_DOWN:
                        render_scene->cam.pitch -= 5;
                        camera_update (&render_scene->cam);
                        break;
                    case SDLK_j:
                    case SDLK_LEFT:
                        render_scene->cam.orbit += 5;
                        camera_update (&render_scene->cam);
                        break;
                    case SDLK_c:
                        scene_center(render_scene);
                        break;
                    case SDLK_k:
                    case SDLK_RIGHT:
                        render_scene->cam.orbit -= 5;
                        camera_update (&render_scene->cam);
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
                set_zoom (-event.wheel.y * 10.0);
        }

        if (mouse_pressed && mouse_moved)
        {
            int mx, my;
            SDL_GetMouseState (&mx, &my);
            float dx = (mouse_pos[0] - mx) * 0.01;
            float dy = (mouse_pos[1] - my) * 0.01;
            render_scene->cam.orbit += dx;
            render_scene->cam.pitch -= dy;
            camera_update (&render_scene->cam);
        }

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        scene_render (render_scene, projection_matrix);

        SDL_GL_SwapWindow (win);
        SDL_Delay (1);

        frame_end();
        SDL_SetWindowTitle (win, frame_rate_string_var);
    }
}

static void set_render_flags()
{
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glCullFace (GL_BACK);
    glEnable (GL_PROGRAM_POINT_SIZE);
}

static void init()
{
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
        die ("Cannot initialize video: %s\n", SDL_GetError());

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
        die ("Cannot create window: %s\n", SDL_GetError());

    ctx = SDL_GL_CreateContext (win);

    if (ctx == NULL)
        die ("Cannot create OpenGL context: %s\n", SDL_GetError());

    SDL_GL_SetSwapInterval (1);

    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT);

    set_render_flags();

    SDL_GL_SwapWindow (win);

    // Define the projection matrix
    mat4x4_perspective (projection_matrix, FOV, (float) WIDTH / (float) HEIGHT, NEAR, FAR);
}

// -------------------------- PRIVATE functions ---------------------------

static void frame_begin()
{
    frame_start_time = SDL_GetTicks();
}

static void frame_end()
{

    frame_end_time = SDL_GetTicks();

    frame_time_var = ( (double) (frame_end_time - frame_start_time) / 1000.0f);
    frame_acc_time += frame_time_var;
    frame_counter++;

    if (frame_acc_time > frame_update_rate)
    {
        frame_rate_var = round ( (double) frame_counter / frame_acc_time);
        frame_counter = 0;
        frame_acc_time = 0.0;
    }

    sprintf (frame_rate_string_var, "rex-view - %i FPS", frame_rate_var);
}
