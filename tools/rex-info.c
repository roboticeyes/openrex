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
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "rex-block-image.h"
#include "rex-block-material.h"
#include "rex-block-mesh.h"
#include "rex-block.h"
#include "rex-header.h"
#include "status.h"
#include "util.h"

static const char *rex_data_types[]
    = { "LineSet", "Text", "Vertex", "Mesh", "Image", "MaterialStandard", "PeopleSimulation", "UnityPackage" };

static const char *rex_image_types[] = { "Raw", "Jpg", "Png" };

void usage (const char *exec)
{
    die ("usage: %s filename.rex\n", exec);
}

void rex_dump_header (struct rex_header *header)
{

    printf ("═══════════════════════════════════════════\n");
    printf ("version                %20d\n", header->version);
    printf ("nr_datablocks          %20d\n", header->nr_datablocks);
    printf ("start_addr             %20d\n", header->start_addr);
    printf ("sz_all_datablocks      %20lu\n", header->sz_all_datablocks);
}

void rex_dump_block_header (struct rex_block *block)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("id                     %20lu\n", block->id);
    printf ("type                   %20s\n", rex_data_types[block->type]);
    printf ("version                %20d\n", block->version);
    printf ("sz                     %20d\n", block->sz);
}

void rex_dump_material_block (struct rex_material_standard *mat)
{
    if (!mat)
        return;

    printf ("Ka %22.2f %5.2f %5.2f %5ld\n", mat->ka_red, mat->ka_green, mat->ka_blue, (mat->ka_textureId != REX_NOT_SET) ? mat->ka_textureId : -1);
    printf ("Kd %22.2f %5.2f %5.2f %5ld\n", mat->kd_red, mat->kd_green, mat->kd_blue, (mat->kd_textureId != REX_NOT_SET) ? mat->kd_textureId : -1);
    printf ("Ks %22.2f %5.2f %5.2f %5ld\n", mat->ks_red, mat->ks_green, mat->ks_blue, (mat->ks_textureId != REX_NOT_SET) ? mat->ks_textureId : -1);
    printf ("ns %40.2f \n", mat->ns);
    printf ("alpha %37.2f \n", mat->alpha);
}

void rex_dump_mesh_block (struct rex_mesh *mesh)
{
    if (!mesh)
        return;

    printf ("name                   %20s\n", (mesh->name) ? mesh->name : "");
    printf ("vertices               %20u\n", mesh->nr_vertices);
    printf ("triangles              %20u\n", mesh->nr_triangles);
    printf ("normals                %20s\n", (mesh->normals) ? "yes" : "no");
    printf ("texture coords         %20s\n", (mesh->tex_coords) ? "yes" : "no");
    printf ("vertex colors          %20s\n", (mesh->colors) ? "yes" : "no");
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 2)
        usage (argv[0]);

    long sz;
    uint8_t *buf = read_file_binary (argv[1], &sz);
    if (buf == NULL)
        die ("Cannot open REX file %s\n", argv[1]);

    struct rex_header header;
    uint8_t *ptr = rex_header_read (buf, &header);
    if (ptr == NULL)
        die ("Cannot read REX header");

    rex_dump_header (&header);

    for (int i = 0; i < header.nr_datablocks; i++)
    {
        struct rex_block block;
        ptr = rex_block_read (ptr, &block);
        rex_dump_block_header (&block);

        if (block.type == Mesh)
        {
            struct rex_mesh *mesh = block.data;
            rex_dump_mesh_block (mesh);
            rex_mesh_free (mesh);
            FREE (block.data);
        }
        else if (block.type == MaterialStandard)
        {
            struct rex_material_standard *mat = block.data;
            rex_dump_material_block (mat);
            FREE (block.data);
        }
        else if (block.type == Image)
        {
            struct rex_image *img = block.data;
            printf ("compression %31s\n", rex_image_types[img->compression]);
            printf ("image size  %31d\n", block.sz);
            FREE (img->data);
            FREE (block.data);
        }
    }
    FREE (buf);
    printf ("═══════════════════════════════════════════\n");
    return 0;
}
