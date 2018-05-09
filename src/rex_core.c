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
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "rex_core.h"
#include "status.h"
#include "util.h"

struct rex_header rex_create_header (void)
{
    struct rex_header header =
    {
        .version = REX_FILE_VERSION,
        .crc = 0,
        .nr_datablocks = 0,
        .start_addr = 0,
        .sz_all_datablocks = 0,
    };

    memcpy (header.magic, REX_FILE_MAGIC, sizeof (REX_FILE_MAGIC));
    memset (header.reserved, 0, 42);
    return header;
}

int rex_read_header (FILE *fp, struct rex_header *header)
{
    FP_CHECK (fp);
    rewind (fp);
    if (fread (header, sizeof (*header), 1, fp) != 1)
        return REX_ERROR_FILE_READ;

    if (strncmp (header->magic, "REX1", 4) != 0)
        return REX_ERROR_WRONG_MAGIC;

    // NOTE: we skip the coordinate system block because it is not used
    // the file pointer is set to beginning of data block
    fseek (fp, header->start_addr, SEEK_SET);

    return REX_OK;
}

int rex_write_header (FILE *fp, struct rex_header *header)
{
    FP_CHECK (fp)
    rewind (fp);

    // write dummy coordinate system block
    struct rex_csb csb = { .srid = 0, .sz = 4, .name = "EPSG", .offset_x = 0, .offset_y = 0, .offset_z = 0 };

    header->start_addr = sizeof (struct rex_header) + sizeof (csb);

    if (fwrite (header, sizeof (*header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;

    assert (ftell (fp) == 64);

    if (fwrite (&csb, sizeof (csb), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    return REX_OK;
}

int rex_read_data_block_header (FILE *fp, struct rex_block_header *header)
{
    FP_CHECK (fp)

    if (fread (header, sizeof (*header), 1, fp) != 1)
        return REX_ERROR_FILE_READ;

    return REX_OK;
}

int rex_read_data_block (FILE *fp, uint8_t *block, uint32_t len)
{
    FP_CHECK (fp)

    if (!block)
    {
        warn ("Block is not allocated");
        return REX_ERROR_MEMORY;
    }
    if (fread (block, sizeof (uint8_t), len, fp) != len)
        return REX_ERROR_FILE_READ;

    return REX_OK;
}

int rex_read_mesh_block (FILE *fp, long block_size, struct rex_mesh_header *header, struct rex_mesh *mesh)
{
    FP_CHECK (fp)

    if (!mesh)
    {
        warn ("Mesh data structure not allocated");
        return REX_ERROR_MEMORY;
    }

    rex_mesh_init (mesh);

    long block_end = ftell (fp) + block_size;

    // read mesh header
    if (fread (header, sizeof (struct rex_mesh_header), 1, fp) != 1)
    {
        fseek (fp, block_end, SEEK_SET);
        return REX_ERROR_FILE_READ;
    }

    mesh->nr_triangles = header->nrTriangles;
    mesh->nr_vertices = header->nrOfVtxCoords;
    mesh->name = strdup (header->name);

    // read positions
    if (mesh->nr_vertices)
    {
        mesh->positions = malloc (mesh->nr_vertices * sizeof (struct rex_position));
        if (fread (mesh->positions, sizeof (struct rex_position), mesh->nr_vertices, fp) != mesh->nr_vertices)
        {
            free (mesh->positions);
            fseek (fp, block_end, SEEK_SET);
            return REX_ERROR_FILE_READ;
        }
    }

    // read normals
    if (header->nrOfNorCoords)
    {
        mesh->normals = malloc (header->nrOfNorCoords * sizeof (struct rex_position));
        if (fread (mesh->normals, sizeof (struct rex_position), header->nrOfNorCoords, fp) != header->nrOfNorCoords)
        {
            free (mesh->normals);
            fseek (fp, block_end, SEEK_SET);
            return REX_ERROR_FILE_READ;
        }
    }

    // read texture coords
    if (header->nrOfTexCoords)
    {
        mesh->tex_coords = malloc (header->nrOfTexCoords * sizeof (struct rex_texel));
        if (fread (mesh->tex_coords, sizeof (struct rex_texel), header->nrOfTexCoords, fp) != header->nrOfTexCoords)
        {
            free (mesh->tex_coords);
            fseek (fp, block_end, SEEK_SET);
            return REX_ERROR_FILE_READ;
        }
    }

    // read colors
    if (header->nrOfVtxColors)
    {
        mesh->colors = malloc (header->nrOfVtxColors * sizeof (struct rex_position));
        if (fread (mesh->colors, sizeof (struct rex_position), header->nrOfVtxColors, fp) != header->nrOfVtxColors)
        {
            free (mesh->colors);
            fseek (fp, block_end, SEEK_SET);
            return REX_ERROR_FILE_READ;
        }
    }

    // read triangles
    if (mesh->nr_triangles)
    {
        mesh->triangles = malloc (mesh->nr_triangles * sizeof (struct rex_triangle));
        if (fread (mesh->triangles, sizeof (struct rex_triangle), mesh->nr_triangles, fp) != mesh->nr_triangles)
        {
            free (mesh->triangles);
            fseek (fp, block_end, SEEK_SET);
            return REX_ERROR_FILE_READ;
        }
    }

    fseek (fp, block_end, SEEK_SET);
    return REX_OK;
}

int rex_write_mesh_block (
    FILE *fp, struct rex_header *header, struct rex_mesh *mesh, uint64_t material_id)
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

    long total_sz = REX_MESH_HEADER_SIZE + mesh->nr_vertices * sizeof (float) * 3 + nr_normals * sizeof (float) * 3
                    + nr_tex_coords * sizeof (float) * 2 + nr_colors * sizeof (float) * 3 + mesh->nr_triangles * sizeof (uint32_t) * 3;

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

int rex_write_material_block (FILE *fp, struct rex_header *header, struct rex_material_standard *mat, uint64_t id)
{
    // write block header
    struct rex_block_header block_header = { .type = 5, .version = 1, .sz = sizeof (*mat), .id = id };

    if (fwrite (&block_header, sizeof (block_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mat, sizeof (*mat), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;

    header->nr_datablocks += 1;
    header->sz_all_datablocks += sizeof (*mat);
    return REX_OK;
}

int rex_write_image_bock (
    FILE *fp, struct rex_header *header, uint8_t *img, uint64_t size, enum rex_image_type type, uint64_t id)
{
    uint64_t total_sz = sizeof (uint32_t) + size;
    uint32_t image_type = type;

    // write block header
    struct rex_block_header block_header = { .type = 4, .version = 1, .sz = total_sz, .id = id };

    if (fwrite (&block_header, sizeof (block_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (&image_type, sizeof (uint32_t), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (img, size, 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;

    header->nr_datablocks += 1;
    header->sz_all_datablocks += total_sz;
    return REX_OK;
}

int rex_read_material_block (FILE *fp, long block_size, struct rex_material_standard *mat)
{
    FP_CHECK (fp)

    if (!mat)
    {
        warn ("Material data structure not allocated");
        return REX_ERROR_MEMORY;
    }

    long block_end = ftell (fp) + block_size;

    if (fread (mat, sizeof (struct rex_material_standard), 1, fp) != 1)
    {
        fseek (fp, block_end, SEEK_SET);
        return REX_ERROR_FILE_READ;
    }

    return REX_OK;
}

int rex_read_image_block (FILE *fp, long block_size, uint32_t *compression, uint8_t *data, uint64_t *data_size)
{
    FP_CHECK (fp)

    long block_end = ftell (fp) + block_size;

    if (fread (compression, sizeof (uint32_t), 1, fp) != 1)
    {
        fseek (fp, block_end, SEEK_SET);
        return REX_ERROR_FILE_READ;
    }

    *data_size = block_size - sizeof (uint32_t);
    data = malloc (*data_size);
    if (fread (data, *data_size, 1, fp) != 1)
    {
        fseek (fp, block_end, SEEK_SET);
        return REX_ERROR_FILE_READ;
    }

    return REX_OK;
}


void rex_mesh_init (struct rex_mesh *mesh)
{
    if (!mesh)
        return;

    mesh->id = 0;
    mesh->nr_vertices = 0;
    mesh->nr_triangles = 0;
    mesh->name = 0;
    mesh->positions = 0;
    mesh->normals = 0;
    mesh->tex_coords = 0;
    mesh->colors = 0;
    mesh->triangles = 0;
}

void rex_mesh_free (struct rex_mesh *mesh)
{
    if (!mesh)
        return;

    if (mesh->name)
        free (mesh->name);
    if (mesh->positions)
        free (mesh->positions);
    if (mesh->normals)
        free (mesh->normals);
    if (mesh->tex_coords)
        free (mesh->tex_coords);
    if (mesh->colors)
        free (mesh->colors);
    if (mesh->triangles)
        free (mesh->triangles);
    rex_mesh_init (mesh);
}
