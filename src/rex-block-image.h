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

/**
 * This is a list of supported image compressions
 */
enum rex_image_compression
{
    Raw24 = 0,
    Jpeg = 1,
    Png = 2
};

struct rex_image
{
    uint32_t compression;
    uint8_t *data;
};

/*
 * Reads a image block from the given pointer. This call will allocate memory
 * for the image. The caller is responsible to free this memory! The sz parameter
 * is required for the number of bytes to read.
 */
uint8_t *rex_block_read_image (uint8_t *ptr, struct rex_image *img, uint32_t sz);

#ifdef __cplusplus
}
#endif

