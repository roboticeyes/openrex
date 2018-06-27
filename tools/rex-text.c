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
 *
 * This file generates a sample REX file with some text information.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rex.h"

struct rex_text data[] =
{
    {
        .position = {0.0f, 0.0f, 0.0f},
        .font_size = 32,
        .data = "Welcome to Robotic Eyes"
    },
    {
        .position = {0.0f, 1.0f, 0.0f},
        .font_size = 24,
        .data = "OpenREX is great!"
    }
};

void usage (const char *exec)
{
    die ("usage: %s filename.rex\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n");
    printf ("Generating sample REX text file ...\n\n");

    if (argc < 2)
        usage (argv[0]);

    FILE *fp = fopen (argv[1], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[1]);

    struct rex_header *header = rex_header_create();

    uint8_t *text_data[LEN(data)];
    long text_data_sz[LEN(data)];
    for (int i = 0; i < LEN(data); i++)
    {
        printf("Adding text: %s\n", data[i].data);
        uint8_t *text_ptr = rex_block_write_text (i, header, &data[i], &text_data_sz[i]);
        text_data[i] = text_ptr;
    }

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    fwrite (header_ptr, header_sz, 1, fp);

    for (int i = 0; i < LEN(data); i++)
    {
        fwrite (text_data[i], text_data_sz[i], 1, fp);
        FREE(text_data[i]);
    }

    fclose (fp);

    FREE (header_ptr);
    FREE (header);
    return 0;
}
