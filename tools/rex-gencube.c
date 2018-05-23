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
#include <stdlib.h>

#include "config.h"
#include "core.h"
#include "global.h"
#include "linmath.h"
#include "util.h"

vec3 vertices[] =
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

vec2 texels[] =
{
    {0, 0},
    {1, 0},
    {1, 1},
    {0, 1},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
};

struct rex_triangle triangles[] =
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
    die ("usage: %s texturefile.png filename.rex\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 3)
        usage (argv[0]);

    FILE *fp = fopen (argv[2], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[2]);

    struct rex_header header = rex_create_header();
    rex_write_header (fp, &header);

    // write texture file
    FILE *t = fopen (argv[1], "rb");
    if (!t)
        die ("Cannot open texture file %s\n", argv[1]);
    fseek (t, 0, SEEK_END);
    uint64_t sz = ftell (t);
    fseek (t, 0, SEEK_SET);
    uint8_t *img = malloc (sz);
    if (fread (img, sz, 1, t) != 1)
        die ("Cannot read image content");
    fclose (t);

    if (rex_write_image_bock (fp, &header, img, sz, Png, 0))
    {
        warn ("Error during file write %d\n", errno);
        FREE (img);
        fclose (fp);
        return -1;
    }

    // write material
    struct rex_material_standard mat =
    {
        .ka_red = 1,
        .ka_green = 0,
        .ka_blue = 0,
        .ka_textureId = 0,
        .kd_red = 1,
        .kd_green = 0,
        .kd_blue = 0,
        .kd_textureId = 0,
        .ks_red = 1,
        .ks_green = 0,
        .ks_blue = 0,
        .ks_textureId = 0,
        .ns = 0,
        .alpha = 1
    };

    if (rex_write_material_block (fp, &header, &mat, 1))
    {
        warn ("Error during file write %d\n", errno);
        fclose (fp);
        return -1;
    }

    struct rex_mesh mesh =
    {
        .id = 2,
        .name = "test",
        .nr_vertices = LEN (vertices),
        .nr_triangles = LEN (triangles),
        .positions = & (vertices[0][0]),
        .normals = NULL,
        .tex_coords = & (texels[0][0]),
        .colors = NULL,
        .triangles = & (triangles[0].v1)
    };

    if (rex_write_mesh_block (fp, &header, &mesh, 1 /* material_id */))
    {
        warn ("Error during file read %d\n", errno);
        fclose (fp);
        return -1;
    }
    rex_write_header (fp, &header);

    fclose (fp);
    FREE (img);
    return 0;
}

