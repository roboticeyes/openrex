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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Represents a complete REX mesh. The reference to the according material is done by the materialId.
 * Please make sure that positions, normals, tex_coords, and colors
 * are vertex-aligned. This means that only one index is referring to all information.
 * However, it is valid to have normals, tex_coords, and colors being NULL.
 */
struct rex_mesh
{
    uint16_t lod;            // level of detail of the geometry
    uint16_t max_lod;

    uint32_t nr_vertices;
    uint32_t nr_triangles;

    float *positions;
    float *normals;
    float *tex_coords;
    float *colors;
    uint32_t *triangles;

    char name[74];           // the mesh name (user-readable)
    uint64_t material_id;    // id which refers to the corresponding material block in this file
};

/**
 * Reads a complete mesh data block. The ptr must point to the beginning of
 * the block. The mesh parameter must not be NULL.
 * Please note that memory will be allocated for the mesh data.
*/
uint8_t *rex_block_read_mesh (uint8_t *ptr, struct rex_mesh *mesh);

void rex_mesh_init (struct rex_mesh *mesh);
void rex_mesh_free (struct rex_mesh *mesh);
void rex_mesh_dump_obj (struct rex_mesh *mesh);

#ifdef __cplusplus
}
#endif

