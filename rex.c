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

int main (int argc, char **argv)
{
    printf ("%s v%s (c) Robotic Eyes\n", app_name, VERSION);

    struct rex_header header;

    FILE *fp = fopen ("data/oscar.rex", "rb");
    int err = rex_read_header (fp, &header);
    rex_dump_header (&header);

    for (int i = 0; i < header.nr_datablocks; i++)
    {
        struct rex_block_header block_header;
        rex_read_data_block_header (fp, &block_header);
        fseek (fp, block_header.sz, SEEK_CUR);
        /* uint8_t *block = malloc(block_header.sz); */
        /* memset(block, 0, block_header.sz); */
        /* rex_read_data_block(fp, block, block_header.sz); */
        rex_dump_block_header (&block_header);
    }

    fclose (fp);
    return 0;
}
