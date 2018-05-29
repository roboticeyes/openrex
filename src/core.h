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

