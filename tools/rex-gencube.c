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
#include <string.h>

#include "config.h"
#include "core.h"
#include "global.h"
#include "linmath.h"
#include "util.h"

float cube_vertices[] =
{
    // front
    -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // top
        -1.0,  1.0,  1.0,
        1.0,  1.0,  1.0,
        1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
        // back
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
        1.0,  1.0, -1.0,
        // bottom
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        // left
        -1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        // right
        1.0, -1.0,  1.0,
        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        1.0,  1.0,  1.0,
    };

float cube_normals[] =
{
    // front
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    // top
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    // back
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    // bottom
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    // left
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    // right
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
};

float cube_texcoords[2 * 4 * 6] =
{
    // front
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
};

uint32_t cube_elements[] =
{
    // front
    0,  1,  2,
    2,  3,  0,
    // top
    4,  5,  6,
    6,  7,  4,
    // back
    8,  9, 10,
    10, 11,  8,
    // bottom
    12, 13, 14,
    14, 15, 12,
    // left
    16, 17, 18,
    18, 19, 16,
    // right
    20, 21, 22,
    22, 23, 20,
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

    struct rex_header *header = rex_header_create();
    rex_header_write (fp, header);

    // setup all texture coordinates for each side
    for (int i = 1; i < 6; i++)
        memcpy (&cube_texcoords[i * 4 * 2], &cube_texcoords[0], 2 * 4 * sizeof (float));

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

    if (rex_write_image_bock (fp, header, img, sz, Png, 0))
    {
        warn ("Error during file write %d\n", errno);
        FREE (img);
        fclose (fp);
        return -1;
    }

    // write material
    struct rex_material_standard mat =
    {
        .ka_red = 0,
        .ka_green = 0,
        .ka_blue = 0,
        .ka_textureId = 0,
        .kd_red = 1,
        .kd_green = 0,
        .kd_blue = 0,
        .kd_textureId = 0,
        .ks_red = 0,
        .ks_green = 0,
        .ks_blue = 0,
        .ks_textureId = 0,
        .ns = 0,
        .alpha = 1
    };

    if (rex_write_material_block (fp, header, &mat, 1))
    {
        warn ("Error during file write %d\n", errno);
        fclose (fp);
        return -1;
    }

    struct rex_mesh mesh =
    {
        .id = 2,
        .name = "test",
        .nr_vertices = LEN (cube_vertices) / 3,
        .nr_triangles = LEN (cube_elements) / 3,
        .positions = cube_vertices,
        .normals = cube_normals,
        .tex_coords = NULL, //& (texels[0][0]),
        .colors = NULL,
        .triangles = cube_elements
    };

    if (rex_write_mesh_block (fp, header, &mesh, 1 /* material_id */))
    {
        warn ("Error during file read %d\n", errno);
        fclose (fp);
        return -1;
    }
    rex_header_write (fp, header);

    fclose (fp);
    FREE (img);
    FREE (header);
    return 0;
}
