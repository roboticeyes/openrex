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
#include <stdio.h>

#include "config.h"

#include "rex_core.h"
#include "util.h"

void usage (const char *exec)
{
    die ("usage: %s filename.rex\n", exec);
}

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

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 2)
        usage (argv[0]);

    struct rex_header header;

    FILE *fp = fopen (argv[1], "rb");

    if (!fp)
        die ("Cannot open REX file %s\n", argv[1]);
    rex_read_header (fp, &header);
    rex_dump_header (&header);

    for (int i = 0; i < header.nr_datablocks; i++)
    {
        struct rex_block_header block_header;
        rex_read_data_block_header (fp, &block_header);
        fseek (fp, block_header.sz, SEEK_CUR);
        rex_dump_block_header (&block_header);
    }

    fclose (fp);
    return 0;
}

