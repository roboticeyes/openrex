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

#include <stdio.h>

#include "rex-block-mesh.h"
#include "global.h"
#include "status.h"
#include "util.h"

uint8_t *rex_block_read_mesh (uint8_t *ptr, struct rex_mesh *mesh)
{
    MEM_CHECK (ptr)
    MEM_CHECK (mesh)

    rex_mesh_init (mesh);

    rexcpy (&mesh->lod, ptr, sizeof (uint16_t));
    rexcpy (&mesh->max_lod, ptr, sizeof (uint16_t));
    rexcpy (&mesh->nr_vertices, ptr, sizeof (uint32_t));

    uint32_t nr_normals;
    uint32_t nr_texcoords;
    uint32_t nr_colors;

    rexcpy (&nr_normals, ptr, sizeof (uint32_t));
    rexcpy (&nr_texcoords, ptr, sizeof (uint32_t));
    rexcpy (&nr_colors, ptr, sizeof (uint32_t));

    rexcpy (&mesh->nr_triangles, ptr, sizeof (uint32_t));

    uint32_t start_coords;
    uint32_t start_normals;
    uint32_t start_texcoords;
    uint32_t start_colors;
    uint32_t start_triangles;

    rexcpy (&start_coords, ptr, sizeof (uint32_t));
    rexcpy (&start_normals, ptr, sizeof (uint32_t));
    rexcpy (&start_texcoords, ptr, sizeof (uint32_t));
    rexcpy (&start_colors, ptr, sizeof (uint32_t));
    rexcpy (&start_triangles, ptr, sizeof (uint32_t));

    rexcpy (&mesh->material_id, ptr, sizeof (uint64_t));

    uint16_t sz; // not used anymore since string is fixed size
    rexcpy (&sz, ptr, sizeof (uint16_t));
    rexcpy (mesh->name, ptr, 74);

    // read positions
    if (mesh->nr_vertices)
    {
        mesh->positions = malloc (mesh->nr_vertices * 12);
        rexcpy (mesh->positions, ptr, mesh->nr_vertices * 12);
    }

    // read normals
    if (nr_normals)
    {
        mesh->normals = malloc (nr_normals * 12);
        rexcpy (mesh->normals, ptr, nr_normals * 12);
    }

    // read texture coords
    if (nr_texcoords)
    {
        mesh->tex_coords = malloc (nr_texcoords * 8);
        rexcpy (mesh->tex_coords, ptr, nr_texcoords * 8);
    }

    // read colors
    if (nr_colors)
    {
        mesh->colors = malloc (nr_colors * 12);
        rexcpy (mesh->colors, ptr, nr_colors * 12);
    }

    // read triangles
    if (mesh->nr_triangles)
    {
        mesh->triangles = malloc (mesh->nr_triangles * 12);
        rexcpy (mesh->triangles, ptr, mesh->nr_triangles * 12);
    }

    return ptr;
}

#if 0
int rex_write_mesh_block (FILE *fp, struct rex_header *header, struct rex_mesh *mesh, uint64_t material_id)
{
    uint32_t nr_normals = (mesh->normals) ? mesh->nr_vertices : 0;
    uint32_t nr_tex_coords = (mesh->tex_coords) ? mesh->nr_vertices : 0;
    uint32_t nr_colors = (mesh->colors) ? mesh->nr_vertices : 0;

    uint64_t ofs_vtx = REX_MESH_HEADER_SIZE; // start vertex block relative to mesh header start
    uint64_t ofs_nor = ofs_vtx + mesh->nr_vertices * sizeof (float) * 3;
    uint64_t ofs_tex = ofs_nor + nr_normals * sizeof (float) * 3;
    uint64_t ofs_col = ofs_tex + nr_tex_coords * sizeof (float) * 2;
    uint64_t ofs_tri = ofs_col + nr_colors * sizeof (float) * 3;

    // create mesh block
    struct rex_mesh_header mesh_header =
    {
        .lod = 0,
        .maxLod = 0,
        .nrOfVtxCoords = mesh->nr_vertices,
        .nrOfNorCoords = nr_normals,
        .nrOfTexCoords = nr_tex_coords,
        .nrOfVtxColors = nr_colors,
        .nrTriangles = mesh->nr_triangles,
        .startVtxCoords = ofs_vtx,
        .startNorCoords = ofs_nor,
        .startTexCoords = ofs_tex,
        .startVtxColors = ofs_col,
        .startTriangles = ofs_tri,
        .materialId = material_id
    };

    strncpy (mesh_header.name, mesh->name, REX_MESH_NAME_MAX_SIZE - 1);
    mesh_header.size = strlen (mesh_header.name);

    long total_sz = REX_MESH_HEADER_SIZE
                    + mesh->nr_vertices * sizeof (float) * 3
                    + nr_normals * sizeof (float) * 3
                    + nr_tex_coords * sizeof (float) * 2
                    + nr_colors * sizeof (float) * 3
                    + mesh->nr_triangles * sizeof (uint32_t) * 3;

    // write block header
    struct rex_block_header block_header = { .type = 3, .version = 1, .sz = total_sz, .id = mesh->id };

    if (fwrite (&block_header, sizeof (block_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (&mesh_header, sizeof (mesh_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mesh->positions, sizeof (float) * 3, mesh->nr_vertices, fp) != mesh->nr_vertices)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mesh->normals, sizeof (float) * 3, nr_normals, fp) != nr_normals)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mesh->tex_coords, sizeof (float) * 2, nr_tex_coords, fp) != nr_tex_coords)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mesh->colors, sizeof (float) * 3, nr_colors, fp) != nr_colors)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mesh->triangles, sizeof (uint32_t) * 3, mesh->nr_triangles, fp) != mesh->nr_triangles)
        return REX_ERROR_FILE_WRITE;

    header->nr_datablocks += 1;
    header->sz_all_datablocks += total_sz + REX_BLOCK_HEADER_SIZE;
    return REX_OK;
}

#endif


void rex_mesh_init (struct rex_mesh *mesh)
{
    if (!mesh) return;

    mesh->lod = 0;
    mesh->max_lod = 0;

    mesh->nr_vertices = 0;
    mesh->nr_triangles = 0;

    mesh->positions = 0;
    mesh->normals = 0;
    mesh->tex_coords = 0;
    mesh->colors = 0;
    mesh->triangles = 0;

    memset (mesh->name, 0, 74);
    mesh->material_id = REX_NOT_SET;
}

void rex_mesh_free (struct rex_mesh *mesh)
{
    if (!mesh) return;

    if (mesh->positions)
        FREE (mesh->positions);
    if (mesh->normals)
        FREE (mesh->normals);
    if (mesh->tex_coords)
        FREE (mesh->tex_coords);
    if (mesh->colors)
        FREE (mesh->colors);
    if (mesh->triangles)
        FREE (mesh->triangles);
    rex_mesh_init (mesh);
}

void rex_mesh_dump_obj (struct rex_mesh *mesh)
{
    if (!mesh) return;

    if (mesh->positions)
    {
        float *p = mesh->positions;
        for (int i = 0; i < mesh->nr_vertices * 3; i += 3)
            printf ("v %f %f %f\n", p[i], p[i + 1], p[i + 2]);
    }

    if (mesh->triangles)
    {
        uint32_t *t = mesh->triangles;
        for (int i = 0; i < mesh->nr_triangles * 3; i += 3)
            printf ("f %d %d %d\n", t[i] + 1, t[i + 1] + 1, t[i + 2] + 1);
    }
}
