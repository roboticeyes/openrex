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

#include "core.h"
#include "global.h"
#include "rex-block.h"
#include "status.h"
#include "util.h"

int rex_write_material_block (FILE *fp, struct rex_header *header, struct rex_material_standard *mat, uint64_t id)
{
    // write block header
    struct rex_block_header block_header = { .type = 5, .version = 1, .sz = sizeof (*mat), .id = id };

    if (fwrite (&block_header, sizeof (block_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (mat, sizeof (*mat), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;

    header->nr_datablocks += 1;
    header->sz_all_datablocks += sizeof (*mat);
    return REX_OK;
}

int rex_write_rexasset_bock (FILE *fp, struct rex_header *header, uint8_t *blob, uint64_t size, const char *name, uint64_t id)
{
    uint16_t name_len = strlen (name);
    uint64_t total_sz = sizeof (uint16_t) + name_len + size;

    // write block header
    struct rex_block_header block_header = { .type = 7, .version = 1, .sz = total_sz, .id = id };

    if (fwrite (&block_header, sizeof (block_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    // write strlen(name) + name
    if (fwrite (&name_len, sizeof (uint16_t), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (name, 1, name_len, fp) != name_len)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (blob, size, 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;

    header->nr_datablocks += 1;
    header->sz_all_datablocks += total_sz;
    return REX_OK;
}


int rex_write_image_bock (
    FILE *fp, struct rex_header *header, uint8_t *img, uint64_t size, enum rex_image_type type, uint64_t id)
{
    uint64_t total_sz = sizeof (uint32_t) + size;
    uint32_t image_type = type;

    // write block header
    struct rex_block_header block_header = { .type = 4, .version = 1, .sz = total_sz, .id = id };

    if (fwrite (&block_header, sizeof (block_header), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (&image_type, sizeof (uint32_t), 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;
    if (fwrite (img, size, 1, fp) != 1)
        return REX_ERROR_FILE_WRITE;

    header->nr_datablocks += 1;
    header->sz_all_datablocks += total_sz;
    return REX_OK;
}

int rex_read_image_block (FILE *fp, long block_size, uint32_t *compression, uint8_t *data, uint64_t *data_size)
{
    FP_CHECK (fp)

    long block_end = ftell (fp) + block_size;

    if (fread (compression, sizeof (uint32_t), 1, fp) != 1)
    {
        fseek (fp, block_end, SEEK_SET);
        return REX_ERROR_FILE_READ;
    }

    *data_size = block_size - sizeof (uint32_t);
    data = malloc (*data_size);
    memset (data, 0, *data_size);
    if (fread (data, *data_size, 1, fp) != 1)
    {
        fseek (fp, block_end, SEEK_SET);
        return REX_ERROR_FILE_READ;
    }

    return REX_OK;
}

