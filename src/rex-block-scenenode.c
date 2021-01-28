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
#include "rex-block-scenenode.h"
#include "rex-block.h"
#include "util.h"


uint8_t* rex_block_write_scenenode(uint64_t id, struct rex_header* header, struct rex_scenenode* scenenode, long* sz)
{
    MEM_CHECK(scenenode)

    * sz = REX_BLOCK_HEADER_SIZE
        + sizeof(uint64_t)                    // geometryId
        + sizeof(REX_SCENENODE_NAME_MAX_SIZE) // name
        + sizeof(float) * 10;                 // translation, rotation, scale

    uint8_t* ptr = malloc(*sz);
    memset(ptr, 0, *sz);
    uint8_t* addr = ptr;

    struct rex_block block = { .type = SceneNode,.version = 1,.sz = *sz - REX_BLOCK_HEADER_SIZE,.id = id };
    ptr = rex_block_header_write(ptr, &block);

    rexcpyr(&scenenode->geometryId, ptr, sizeof(uint64_t));
    rexcpyr(&scenenode->name, ptr, sizeof(REX_SCENENODE_NAME_MAX_SIZE));
    rexcpyr(&scenenode->tx, ptr, sizeof(float));
    rexcpyr(&scenenode->ty, ptr, sizeof(float));
    rexcpyr(&scenenode->tz, ptr, sizeof(float));
    rexcpyr(&scenenode->rx, ptr, sizeof(float));
    rexcpyr(&scenenode->ry, ptr, sizeof(float));
    rexcpyr(&scenenode->rz, ptr, sizeof(float));
    rexcpyr(&scenenode->rw, ptr, sizeof(float));
    rexcpyr(&scenenode->sx, ptr, sizeof(float));
    rexcpyr(&scenenode->sy, ptr, sizeof(float));
    rexcpyr(&scenenode->sz, ptr, sizeof(float));

    if (header)
    {
        header->nr_datablocks += 1;
        header->sz_all_datablocks += *sz;
    }
    return addr;
}


uint8_t* rex_block_read_scenenode(uint8_t* ptr, struct rex_scenenode* scenenode)
{
    MEM_CHECK(ptr)
    MEM_CHECK(scenenode)

    rexcpy(&scenenode->geometryId, ptr, sizeof(uint64_t));
    rexcpy(&scenenode->name, ptr, REX_SCENENODE_NAME_MAX_SIZE);
    rexcpy(&scenenode->tx, ptr, sizeof(float));
    rexcpy(&scenenode->ty, ptr, sizeof(float));
    rexcpy(&scenenode->tz, ptr, sizeof(float));
    rexcpy(&scenenode->rx, ptr, sizeof(float));
    rexcpy(&scenenode->ry, ptr, sizeof(float));
    rexcpy(&scenenode->rz, ptr, sizeof(float));
    rexcpy(&scenenode->rw, ptr, sizeof(float));
    rexcpy(&scenenode->sx, ptr, sizeof(float));
    rexcpy(&scenenode->sy, ptr, sizeof(float));
    rexcpy(&scenenode->sz, ptr, sizeof(float));

    return ptr;
}