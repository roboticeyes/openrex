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
 * Reads a material block from the data pointer. NULL is returned in case of error,
 * else the pointer after the block is returned.
 */
uint8_t *rex_block_read_material (uint8_t *ptr, struct rex_material_standard *mat);

/**
 * Writes a material block to a binary stream. Memory will be allocated and the caller
 * must take care of releasing the memory. The rex_header can be NULL.
 */
uint8_t *rex_block_write_material(uint64_t id, struct rex_header *header, struct rex_material_standard *mat, long *sz);

#ifdef __cplusplus
}
#endif
