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
#include "rex-header.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rex_lineset
{
    float red;
    float green;
    float blue;
    uint32_t nr_vertices;
    float *vertices;
};

/*
 * Reads a lineset block from the given pointer. This call will allocate memory
 * for the vertices. The caller is responsible to free this memory!
 */
uint8_t *rex_block_read_lineset (uint8_t *ptr, struct rex_lineset *lineset);

/**
 * Writes a lineset block to binary. Memory will be allocated and the caller
 * must take care of releasing the memory. The rex_header can be NULL.
 */
uint8_t *rex_block_write_lineset (uint64_t id, struct rex_header *header, struct rex_lineset *lineset, long *sz);

#ifdef __cplusplus
}
#endif

