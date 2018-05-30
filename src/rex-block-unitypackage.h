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

struct rex_unitypackage
{
    uint16_t target_platform;
    uint16_t unity_version;
    uint8_t *data;
    uint64_t sz; // size in bytes of the package
};

/*
 * Reads an Unity asset bundle block from the data pointer. NULL is returned in case of error,
 * else the pointer after the block is returned.
 */
uint8_t *rex_block_read_unitypackage (uint8_t *ptr, struct rex_unitypackage *unity);

/**
 * Writes a Unity asset bundle block to a binary stream. Memory will be allocated and the caller
 * must take care of releasing the memory. The rex_header can be NULL.
 */
uint8_t *rex_block_write_unitypackage (uint64_t id, struct rex_header *header, struct rex_unitypackage *unity, long *sz);

#ifdef __cplusplus
}
#endif

