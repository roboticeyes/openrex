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
  * \brief REX track block storing a list of transformations at a given point in time
  *
  * | **size [bytes]** | **name**     | **type** | **description**                    |
  * |------------------|--------------|----------|------------------------------------|
  * | 4                | nrOfPoints   | uint32   | number of points                   |
  * | 8                | timestamp    | int64    | timestamp (UNIX time)              |
  * | 4                | x            | float    | x-coordinate of first point        |
  * | 4                | y            | float    | y-coordinate of first point        |
  * | 4                | z            | float    | z-coordinate of first point        |
  * | 4                | nx           | float    | x-coordinate of first normal       |
  * | 4                | ny           | float    | y-coordinate of first normal       |
  * | 4                | nz           | float    | z-coordinate of first normal       |
  * | 4                | confidence   | float    | tracking confidence of first point |
  * | 4                | x            | float    | x-coordinate of second point       |
  * | ...              |              |          |                                    |
  */

#include <stdint.h>
#include "rex-header.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* Stores all the properties for a REX track
*/
struct rex_track
{
    uint32_t nr_points;     //!< the number of points stored in points, normals and confidences
    uint64_t timestamp;     //!< timestamp (UNIX time)
    float* points;          //!< the raw data of all points (x0y0z0x1y1...)
    float* normals;         //!< the raw data of all normals (nx0ny0nz0nx1ny1...)
    float* confidences;     //!< confidence value for each point
};

/**
 * Reads a track block from the given pointer. This call will allocate memory
 * for the points, normals and confidences. The caller is responsible to free this memory!
 *
 * \param ptr pointer to the block start
 * \param track the rex_track structure which gets filled
 * \return the pointer to the memory block after the rex_track block
 */
uint8_t* rex_block_read_track(uint8_t* ptr, struct rex_track* track);

/**
 * Writes a track block to binary. Memory will be allocated and the caller
 * must take care of releasing the memory.
 *
 * \param id the data block ID
 * \param header the REX header which gets modified according the the new block, can be NULL
 * \param track the track which should get serialized
 * \param sz the total size of the of the data block which is returned
 * \return a pointer to the data block
 */
uint8_t* rex_block_write_track(uint64_t id, struct rex_header* header, struct rex_track* track, long* sz);

#ifdef __cplusplus
}
#endif
