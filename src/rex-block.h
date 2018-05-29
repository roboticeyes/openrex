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

enum rex_block_type
{
    LineSet          = 0,
    Text             = 1,
    Vertex           = 2,
    Mesh             = 3,
    Image            = 4,
    MaterialStandard = 5,
    PeopleSimulation = 6,
    UnityPackage     = 7
};

struct rex_block
{
    uint16_t type;    // identifies the block and therefore can be used to map *data
    uint16_t version; // block version
    uint32_t sz;      // data block size w/o header
    uint64_t id;      // a unique identifier for this block
    void     *data;   // stores the actual data
};

/*
 * Read the complete data block from the given data block pointer.
 * After successful read, the new pointer location is return, else NULL.
 *
 * Memory is allocated for the data block. The caller must make sure that this
 * memory is then deallocated.
 */
uint8_t *rex_block_read (uint8_t *ptr, struct rex_block *block);

#ifdef __cplusplus
}
#endif
