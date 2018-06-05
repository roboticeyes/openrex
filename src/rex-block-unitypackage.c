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
#include "rex-block-unitypackage.h"
#include "rex-block.h"
#include "util.h"

uint8_t *rex_block_write_unitypackage (uint64_t id, struct rex_header *header, struct rex_unitypackage *unity, long *sz)
{
    MEM_CHECK (unity)
    MEM_CHECK (unity->data)

    *sz = REX_BLOCK_HEADER_SIZE + sizeof (uint16_t) + sizeof (uint16_t) + unity->sz;

    uint8_t *ptr = malloc (*sz);
    memset (ptr, 0, *sz);
    uint8_t *addr = ptr;

    struct rex_block block = { .type = UnityPackage, .version = 1, .sz = *sz - REX_BLOCK_HEADER_SIZE, .id = id };
    ptr = rex_block_header_write (ptr, &block);

    rexcpyr (&unity->target_platform, ptr, sizeof (uint16_t));
    rexcpyr (&unity->unity_version, ptr, sizeof (uint16_t));
    rexcpyr (unity->data, ptr, unity->sz);

    if (header)
    {
        header->nr_datablocks += 1;
        header->sz_all_datablocks += *sz;
    }
    return addr;
}

uint8_t *rex_block_read_unitypackage (uint8_t *ptr, struct rex_unitypackage *unity)
{
    MEM_CHECK (ptr)
    MEM_CHECK (unity)

    rexcpy (&unity->target_platform, ptr, sizeof (uint16_t));
    rexcpy (&unity->unity_version, ptr, sizeof (uint16_t));

    unity->data = malloc (unity->sz);
    rexcpy (unity->data, ptr, unity->sz);
    return ptr;
}
