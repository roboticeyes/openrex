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
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "rex_core.h"
#include "status.h"
#include "util.h"

void rex_dump_header (struct rex_header *header)
{

    printf ("═══════════════════════════════════════════\n");
    printf ("version                %20d\n", header->version);
    printf ("nr_datablocks          %20d\n", header->nr_datablocks);
    printf ("start_addr             %20d\n", header->start_addr);
    printf ("sz_all_datablocks      %20llu\n", header->sz_all_datablocks);
    printf ("═══════════════════════════════════════════\n");
}

void rex_dump_block_header (struct rex_block_header *header)
{
    printf ("id                     %20llu\n", header->id);
    printf ("type                   %20s\n", rex_data_types[header->type]);
    printf ("version                %20d\n", header->version);
    printf ("sz                     %20d\n", header->sz);
    printf ("═══════════════════════════════════════════\n");
}

int rex_read_header (FILE *fp, struct rex_header *header)
{
    assert (fp);
    rewind (fp);
    if (fread (header, sizeof (*header), 1, fp) != 1)
    {
        return REX_ERROR_FILE_READ;
    }

    if (strncmp (header->magic, "REX1", 4) != 0)
    {
        return REX_ERROR_WRONG_MAGIC;
    }

    // NOTE: we skip the coordinate system block because it is not used
    // the file pointer is set to beginning of data block
    fseek (fp, header->start_addr, SEEK_SET);

    return REX_OK;
}

int rex_read_data_block_header (FILE *fp, struct rex_block_header *header)
{
    if (fread (header, sizeof (*header), 1, fp) != 1)
    {
        return REX_ERROR_FILE_READ;
    }

    return REX_OK;
}

int rex_read_data_block (FILE *fp, uint8_t *block, uint32_t len)
{
    if (!block)
    {
        warn ("Block is not allocated");
        return REX_ERROR_MEMORY;
    }
    if (fread (block, sizeof (u_int8_t), len, fp) != len)
    {
        return REX_ERROR_FILE_READ;
    }

    return REX_OK;
}
