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

/**
 * \file
 * \brief REX unity package block storing a fixed pre-compiled Unity package
 *
 * The UnityPackage data block contains an arbitrary pre-prepared Unity package. An example is to store animation data
 * which can then directly be used by the Unity app to be included. The data block size in the header refers to the total
 * size of this block plus the additional two fields. E.g.
 *
 * | **size [bytes]** | **name**        | **type**  | **description**                                              |
 * |------------------|-----------------|-----------|--------------------------------------------------------------|
 * | 2                | target platform | uint16_t  | target platform for the asset package                        |
 * | 2                | unity version   | uint16_t  | Unity version that was used to build the assetbundle (at least 20180)    |
 * |                  | data            | bytes     | data of the unity asset content                              |
 *
 * The current values for target platform are:
 *
 * | **value** | **platform** |
 * |-----------|--------------|
 * | 0         | Android      |
 * | 1         | iOS          |
 * | 2         | WSA          |
 *
 * Please note that the **UnityPackage is only supported by REX Holo**. Android and iOS cannot be used because
 * of some Unity bugs in the asset handling.
 */

#include <stdint.h>
#include "rex-header.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The REX structure storing a pre-compiled Unity package
 */
struct rex_unitypackage
{
    uint16_t target_platform;  //<! the target platform ID (see above)
    uint16_t unity_version;    //<! the unity version stored as an uint16_t
    uint8_t *data;             //<! the actual binary data
    uint64_t sz;               //<! size in bytes of the package
};

/**
 * Reads an Unity asset bundle block from the data pointer. NULL is returned in case of error,
 * else the pointer after the block is returned.
 *
 * \param ptr pointer to the block start
 * \param unity the rex_unitypackage structure which gets filled
 * \return the pointer to the memory block after the rex_unitypackage block
 */
uint8_t *rex_block_read_unitypackage (uint8_t *ptr, struct rex_unitypackage *unity);

/**
 * Writes a Unity asset bundle block to a binary stream. Memory will be allocated and the caller
 * must take care of releasing the memory.
 *
 * \param id the data block ID
 * \param header the REX header which gets modified according the the new block, can be NULL
 * \param unity the Unity package which should get serialized
 * \param sz the total size of the of the data block which is returned
 * \return a pointer to the data block
 */
uint8_t *rex_block_write_unitypackage (uint64_t id, struct rex_header *header, struct rex_unitypackage *unity, long *sz);

#ifdef __cplusplus
}
#endif
