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

#include "rex-header.h"

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

struct rex_material_standard
{
    float    ka_red;       // RED component for ambient color
    float    ka_green;     // GREEN component for ambient color
    float    ka_blue;      // BLUE component for ambient color
    uint64_t ka_textureId; // dataId of the referenced texture for ambient component
    float    kd_red;       // RED component for diffuse color
    float    kd_green;     // GREEN component for diffuse color
    float    kd_blue;      // BLUE component for diffuse color
    uint64_t kd_textureId; // dataId of the referenced texture for diffuse component
    float    ks_red;       // RED component for specular color
    float    ks_green;     // GREEN component for specular color
    float    ks_blue;      // BLUE component for specular color
    uint64_t ks_textureId; // dataId of the referenced texture for specular component
    float    ns;           // specular exponent
    float    alpha;        // alpha between 0..1, 1 means full opaque
};

/*
 * A help structure which allows to set all the required information about a
 * REX mesh. Please make sure that positions, normals, tex_coords, and colors
 * are vertex-aligned. This means that only one index is referring to all information.
 * However, it is valid to have normals, tex_coords, and colors being NULL.
 */
struct rex_mesh
{
    uint64_t id;
    char *name;
    uint32_t nr_vertices;
    uint32_t nr_triangles;
    float *positions;
    float *normals;
    float *tex_coords;
    float *colors;
    uint32_t *triangles;
};

struct rex_triangle
{
    uint32_t v1;
    uint32_t v2;
    uint32_t v3;
};

/**
 * This is a list of supported image types
 */
enum rex_image_type
{
    Raw24 = 0,
    Jpeg = 1,
    Png = 2
};

/**
 * Reads a complete mesh data block. The fp must point to the beginning of
 * the block. The mesh header and the mesh itself will be returned. Please
 * note that memory will be allocated for the mesh data which needs to be
 * deallocated by the caller. block_size is the total size of the block. After
 * reading, fp will be set to the end of the block.
*/
int rex_read_mesh_block (FILE *fp, long block_size, struct rex_mesh_header *header, struct rex_mesh *mesh);

/*
 * Writes a complete mesh block to the given file pointer
 */
int rex_write_mesh_block (FILE *fp, struct rex_header *header, struct rex_mesh *mesh, uint64_t material_id);

/*
 * Reads a mesh material block from the given file pointer
 */
int rex_read_material_block (FILE *fp, long block_size, struct rex_material_standard *mat);

/*
 * Writes a mesh material block to the given file pointer
 */
int rex_write_material_block (FILE *fp, struct rex_header *header, struct rex_material_standard *mat, uint64_t id);

/**
 * Writes a image block which can be used as mesh texture
 * \param fp file pointer
 * \param header rex header
 * \param img the image raw data in memory
 * \param size the size of the binary image in bytes
 * \param type the image type/compression
 * \param id the data id for this block
 */
int rex_write_image_bock (FILE *fp, struct rex_header *header, uint8_t *img, uint64_t size, enum rex_image_type type, uint64_t id);

/**
 * Writes a rex asset block
 * \param fp file pointer
 * \param header rex header
 * \param blob the content of the rex asset file
 * \param size the size of the binary image in bytes
 * \param id the data id for this block
 */
int rex_write_rexasset_bock (FILE *fp, struct rex_header *header, uint8_t *img, uint64_t size, const char *name, uint64_t id);

/*
 * Reads a image block from the given file pointer. This call will allocate memory
 * for the image. The caller is responsible to free this memory!
 */
int rex_read_image_block (FILE *fp, long block_size, uint32_t *compression, uint8_t *data, uint64_t *data_size);

void rex_mesh_init (struct rex_mesh *mesh);
void rex_mesh_free (struct rex_mesh *mesh);
void rex_mesh_dump_obj (struct rex_mesh *mesh);
