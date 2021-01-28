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

#include "global.h"
#include "rex-block-track.h"
#include "rex-block.h"
#include "util.h"


uint8_t* rex_block_write_track(uint64_t id, struct rex_header* header, struct rex_track* track, long* sz)
{
    MEM_CHECK(track)
    MEM_CHECK(track->points)
    MEM_CHECK(track->normals)
    MEM_CHECK(track->confidences)

    * sz = REX_BLOCK_HEADER_SIZE
        + sizeof(uint32_t)  // nr_points
        + sizeof(uint64_t)  // timestamp
        + track->nr_points * 7 * sizeof(float);

    uint8_t* ptr = malloc(*sz);
    memset(ptr, 0, *sz);
    uint8_t* addr = ptr;

    struct rex_block block = { .type = Track,.version = 1,.sz = *sz - REX_BLOCK_HEADER_SIZE,.id = id };
    ptr = rex_block_header_write(ptr, &block);

    rexcpyr(&track->nr_points, ptr, sizeof(uint32_t));
    rexcpyr(&track->timestamp, ptr, sizeof(uint64_t));

    for (int i = 0; i < track->nr_points; i++)
    {
        rexcpyr(track->points, ptr, sizeof(float) * 3);
        rexcpyr(track->normals, ptr, sizeof(float) * 3);
        rexcpyr(track->confidences, ptr, sizeof(float));
    }

    if (header)
    {
        header->nr_datablocks += 1;
        header->sz_all_datablocks += *sz;
    }
    return addr;
}


uint8_t* rex_block_read_track(uint8_t* ptr, struct rex_track* track)
{
    MEM_CHECK(ptr)
    MEM_CHECK(track)

    rexcpy(&track->nr_points, ptr, sizeof(uint32_t));
    rexcpy(&track->timestamp, ptr, sizeof(uint64_t));

    track->points = malloc(track->nr_points * sizeof(float) * 3);
    track->normals = malloc(track->nr_points * sizeof(float) * 3);
    track->confidences = malloc(track->nr_points * sizeof(float));

    for (int i = 0; i < track->nr_points; i++)
    {
        rexcpy(track->points, ptr, sizeof(float) * 3);
        rexcpy(track->normals, ptr, sizeof(float) * 3);
        rexcpy(track->confidences, ptr, sizeof(float));
    }
    
    return ptr;
}
