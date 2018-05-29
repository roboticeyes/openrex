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

