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

#include <stdio.h>

#include "rex-data-block.h"
#include "status.h"
#include "util.h"

int rex_block_header_read (FILE *fp, struct rex_block_header *header)
{
    FP_CHECK (fp);
    if (!header) return REX_ERROR_MEMORY;

    rex_read(&header->type, sizeof(uint16_t), 1, fp);
    rex_read(&header->version, sizeof(uint16_t), 1, fp);
    rex_read(&header->sz, sizeof(uint32_t), 1, fp);
    rex_read(&header->id, sizeof(uint64_t), 1, fp);

    return REX_OK;
}

int rex_block_read (FILE *fp, uint8_t *block, uint32_t len)
{
    FP_CHECK (fp)

    if (!block)
    {
        warn ("Block is not allocated");
        return REX_ERROR_MEMORY;
    }
    if (fread (block, sizeof (uint8_t), len, fp) != len)
        return REX_ERROR_FILE_READ;

    return REX_OK;
}
