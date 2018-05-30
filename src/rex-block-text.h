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
#include "linmath.h"
#include "rex-header.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rex_text
{
    vec3 position;
    float font_size;
    char *data; // null terminated string
};

/*
 * Reads a text block from the given pointer. This call will allocate memory
 * for the text. The caller is responsible to free this memory!
 */
uint8_t *rex_block_read_text (uint8_t *ptr, struct rex_text *text);

/**
 * Writes a text block to binary. Memory will be allocated and the caller
 * must take care of releasing the memory. The rex_header can be NULL.
 */
uint8_t *rex_block_write_text (uint64_t id, struct rex_header *header, struct rex_text *text, long *sz);

#ifdef __cplusplus
}
#endif

