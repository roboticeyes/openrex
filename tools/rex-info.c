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

#include "rex_core.h"
#include "status.h"
#include "util.h"

static const char *rex_data_types[] =
{
    "LineSet",
    "Text",
    "Vertex",
    "Mesh",
    "Image",
    "MaterialStandard",
    "PeopleSimulation",
    "UnityPackage"
};

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
    printf ("═══════════════════════════════════════════\n");
}

void rex_dump_block_header (struct rex_block_header *header)
{
    printf ("id                     %20lu\n", header->id);
    printf ("type                   %20s\n", rex_data_types[header->type]);
    printf ("version                %20d\n", header->version);
    printf ("sz                     %20d\n", header->sz);
    printf ("═══════════════════════════════════════════\n");
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

#if 0
    float *p = mesh->positions;
    for (int i = 0; i < mesh->nr_vertices * 3; i += 3)
        printf ("v %f %f %f\n", p[i], p[i + 1], p[i + 2]);
    uint32_t *t = mesh->triangles;
    for (int i = 0; i < mesh->nr_triangles * 3; i += 3)
        printf ("f %d %d %d\n", t[i] + 1, t[i + 1] + 1, t[i + 2] + 1);
#endif
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 2)
        usage (argv[0]);

    struct rex_header header;

    FILE *fp = fopen (argv[1], "rb");

    if (!fp)
        die ("Cannot open REX file %s\n", argv[1]);
    rex_read_header (fp, &header);
    rex_dump_header (&header);

    for (int i = 0; i < header.nr_datablocks; i++)
    {
        struct rex_block_header block_header;
        rex_read_data_block_header (fp, &block_header);
        if (block_header.type == 3)
        {
            struct rex_mesh_header header;
            struct rex_mesh mesh = {0};
            mesh.id = block_header.id;
            rex_read_mesh_block (fp, block_header.sz, &header, &mesh);
            rex_dump_mesh_block (&mesh);
            rex_mesh_free (&mesh);
        }
        else
            fseek (fp, block_header.sz, SEEK_CUR);

        rex_dump_block_header (&block_header);
    }

    fclose (fp);
    return 0;
}
