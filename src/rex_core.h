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

#include <stdio.h>
#include <stdint.h>

#pragma pack (1)
struct rex_header
{
    char magic[4];
    uint16_t version;
    uint32_t crc;
    uint16_t nr_datablocks;
    uint16_t start_addr;
    uint64_t sz_all_datablocks;
    char reserved[42];
};

struct rex_block_header
{
    uint16_t type;
    uint16_t version;
    uint32_t sz;  // data block size w/o header
    uint64_t id;
};
#pragma pack ()

static const char *rex_data_types[] =
{
    "LineSet",
    "Text",
    "Vertex",
    "Mesh",
    "Image",
    "MaterialStandard",
    "PeopleSimulation",
    "UnityPackage"
};

int rex_read_header (FILE *fp, struct rex_header *header);
void rex_dump_header (struct rex_header *header);

/*
 * Read a data block header from the current file position
 */
int rex_read_data_block_header (FILE *fp, struct rex_block_header *header);

/*
 * Read the data block from the current file position with the given size len
 */
int rex_read_data_block (FILE *fp, uint8_t *block, uint32_t len);

