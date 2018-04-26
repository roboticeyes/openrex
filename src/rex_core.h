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
#pragma once

#include <stdio.h>
#include <stdint.h>

#pragma pack (1)
struct rex_header
{
    char magic[4];
    uint16_t version;
    uint32_t crc;
    uint16_t nr_datablocks;
    uint16_t start_addr;
    uint64_t sz_all_datablocks;
    char reserved[42];
};

struct rex_csb
{
    uint32_t srid;
    uint16_t sz;
    char name[4]; // restricted to 4 bytes here
    float offset_x;
    float offset_y;
    float offset_z;
};

struct rex_block_header
{
    uint16_t type;
    uint16_t version;
    uint32_t sz;  // data block size w/o header
    uint64_t id;
};

struct rex_mesh_header
{
    uint16_t lod;            // level of detail for the given geometry
    uint16_t maxLod;         // maximal level of detail for given geometry
    uint32_t nrOfVtxCoords;  // number of vertex coordinates
    uint32_t nrOfNorCoords;  // number of normal coordinates (can be zero)
    uint32_t nrOfTexCoords;  // number of texture coordinates (can be zero)
    uint32_t nrOfVtxColors;  // number of vertex colors (can be zero)
    uint32_t nrTriangles;    // number of triangles
    uint32_t startVtxCoords; // start vertex coordinate block (relative to mesh block start)
    uint32_t startNorCoords; // start vertex normals block (relative to mesh block start)
    uint32_t startTexCoords; // start of texture coordinate block (relative to mesh block start)
    uint32_t startVtxColors; // start of colors block (relative to mesh block start)
    uint32_t startTriangles; // start triangle block for vertices (relative to mesh block start)
    uint64_t materialId;     // id which refers to the corresponding material block in this file
    uint16_t size;           // size of the following string name
    char name[74];           // the mesh name (user-readable)
};
#pragma pack ()

/*
 * A help structure which allows to set all the required information about a
 * REX mesh. Please make sure that positions, normals, tex_coords, and colors
 * are vertex-aligned. This means that only one index is referring to all information.
 * However, it is valid to have normals, tex_coords, and colors being NULL.
 */
struct rex_mesh {
    uint64_t id;
    uint32_t nr_vertices;
    uint32_t nr_triangles;
    float *positions;
    float *normals;
    float *tex_coords;
    float *colors;
    uint32_t *triangles;
};

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

/**
 * Creates a valid REX header block
 */
struct rex_header rex_create_header();

/*
 * Reads the REX header from an open file pointer
 */
int rex_read_header (FILE *fp, struct rex_header *header);

/*
 * Write the REX header to an open file pointer
 */
int rex_write_header (FILE *fp, struct rex_header *header);

/*
 * Reads a data block header from the current file position
 */
int rex_read_data_block_header (FILE *fp, struct rex_block_header *header);

/*
 * Reads the data block from the current file position with the given size len
 */
int rex_read_data_block (FILE *fp, uint8_t *block, uint32_t len);

/*
 * Writes a complete mesh block to the give file pointer
 */
int rex_write_mesh_block (FILE *fp, struct rex_header *header, struct rex_mesh *mesh, uint64_t material_id, const char *name);
