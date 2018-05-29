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

#include "rex-block.h"
#include "rex-block-mesh.h"
#include "rex-block-material.h"
#include "status.h"
#include "util.h"

uint8_t *rex_block_read (uint8_t *ptr, struct rex_block *block)
{
    MEM_CHECK (ptr);
    MEM_CHECK (block);

    rexcpy (&block->type,    ptr, sizeof (uint16_t));
    rexcpy (&block->version, ptr, sizeof (uint16_t));
    rexcpy (&block->sz,      ptr, sizeof (uint32_t));
    rexcpy (&block->id,      ptr, sizeof (uint64_t));

    uint8_t *data_start = ptr;

    switch (block->type)
    {
        case LineSet:
            warn("LineSet is not yet implemented");
            return  data_start + block->sz;
            break;
        case Text:
            warn("Text is not yet implemented");
            return  data_start + block->sz;
            break;
        case Vertex:
            warn("Vertex is not yet implemented");
            return  data_start + block->sz;
            break;
        case Mesh:
            {
                struct rex_mesh *mesh = malloc(sizeof(struct rex_mesh));
                ptr = rex_block_read_mesh(ptr, mesh);
                block->data = mesh;
                break;
            }
        case Image:
            warn("Images is not yet implemented", block->type);
            return  data_start + block->sz;
            break;
        case MaterialStandard:
            {
                struct rex_material_standard *mat = malloc(sizeof(struct rex_material_standard));
                ptr = rex_block_read_material(ptr, mat);
                block->data = mat;
                break;
            }
            break;
        case PeopleSimulation:
            warn("PeopleSimulation is not yet implemented");
            return  data_start + block->sz;
            break;
        case UnityPackage:
            warn("UnityPackage is not yet implemented");
            return  data_start + block->sz;
            break;
        default:
            warn("Not supported REX block, skipping.");
            return  data_start + block->sz;
    }
    return ptr;
}
