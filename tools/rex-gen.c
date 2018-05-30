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
 *
 * This file generates a sample cube with texture and normal information.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rex.h"
#include "texture.h"

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
    die ("usage: %s filename.rex\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n");
    printf ("Generating sample REX cube with texture ...\n\n");

    if (argc < 2)
        usage (argv[0]);

    FILE *fp = fopen (argv[1], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[1]);

    // setup all texture coordinates for each side
    for (int i = 1; i < 6; i++)
        memcpy (&cube_texcoords[i * 4 * 2], &cube_texcoords[0], 2 * 4 * sizeof (float));

    struct rex_header *header = rex_header_create();

    // write texture
    struct rex_image img;
    img.compression = Png;
    img.data = texture_png;
    img.sz = texture_png_len;
    long img_sz;
    uint8_t *img_ptr = rex_block_write_image (0 /*id*/, header, &img, &img_sz);

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
    long mat_sz;
    uint8_t *mat_ptr = rex_block_write_material (1 /*id*/, header, &mat, &mat_sz);

    // write mesh
    struct rex_mesh mesh =
    {
        .lod = 0,
        .max_lod = 0,
        .name = "Cube",
        .material_id = 1,
        .nr_vertices = LEN (cube_vertices) / 3,
        .nr_triangles = LEN (cube_elements) / 3,
        .positions = cube_vertices,
        .normals = cube_normals,
        .tex_coords = cube_texcoords,
        .colors = NULL,
        .triangles = cube_elements
    };
    long mesh_sz;
    uint8_t *mesh_ptr = rex_block_write_mesh (2 /*id*/, header, &mesh, &mesh_sz);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (img_ptr, img_sz, 1, fp);
    fwrite (mat_ptr, mat_sz, 1, fp);
    fwrite (mesh_ptr, mesh_sz, 1, fp);
    fclose (fp);

    FREE (img_ptr);
    FREE (mat_ptr);
    FREE (mesh_ptr);
    FREE (header_ptr);
    return 0;
}
