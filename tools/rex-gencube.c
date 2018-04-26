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
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "config.h"

#include "rex_core.h"
#include "util.h"
#include "global.h"

#pragma pack (1)
struct vertex
{
    float x;
    float y;
    float z;
};
struct triangle
{
    uint32_t v1;
    uint32_t v2;
    uint32_t v3;
};
#pragma pack ()

struct vertex vertices[] =
{
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 1},
    {0, 1, 0},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 1},
    {1, 1, 0}
};

struct triangle triangles[] =
{
    {0, 1, 3},
    {7, 6, 4},
    {0, 4, 1},
    {1, 5, 2},
    {2, 6, 3},
    {3, 7, 0},
    {1, 2, 3},
    {6, 5, 4},
    {4, 5, 1},
    {5, 6, 2},
    {6, 7, 3},
    {7, 4, 0}
};

void usage (const char *exec)
{
    die ("usage: %s filename.rex\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 2)
        usage (argv[0]);

    FILE *fp = fopen (argv[1], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[1]);

    struct rex_header header = rex_create_header();
    rex_write_header (fp, &header);

    // write material
    struct rex_material_standard mat =
    {
        .ka_red = 1,
        .ka_green = 0,
        .ka_blue = 0,
        .ka_textureId = REX_NOT_SET,
        .kd_red = 1,
        .kd_green = 0,
        .kd_blue = 0,
        .kd_textureId = REX_NOT_SET,
        .ks_red = 1,
        .ks_green = 0,
        .ks_blue = 0,
        .ks_textureId = REX_NOT_SET,
        .ns = 0,
        .alpha = 1
    };

    if (rex_write_material_block (fp, &header, &mat, 0))
    {
        warn ("Error during file read %d\n", errno);
        fclose (fp);
        return -1;
    }

    struct rex_mesh mesh =
    {
        .id = 1,
        .nr_vertices = LEN (vertices),
        .nr_triangles = LEN (triangles),
        .positions = & (vertices[0].x),
        .normals = NULL,
        .tex_coords = NULL,
        .colors = NULL,
        .triangles = & (triangles[0].v1)
    };

    if (rex_write_mesh_block (fp, &header, &mesh, 0, "test"))
    {
        warn ("Error during file read %d\n", errno);
        fclose (fp);
        return -1;
    }
    rex_write_header (fp, &header);

    fclose (fp);
    return 0;
}
