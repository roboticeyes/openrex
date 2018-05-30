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
 * This tool takes a REX Unity Asset file and generates a REX file.
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "rex.h"

void usage (const char *exec)
{
    die ("usage: %s <rex_asset_file> filename.rex <5_digit_unity_version>\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 4)
        usage (argv[0]);

    uint16_t unity_version = atoi(argv[3]);

    if (unity_version < 20180)
        die("Unity version has to be at least 20180");

    FILE *fp = fopen(argv[2], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[2]);

    // read asset bundle file
    long sz;
    uint8_t *buf = read_file_binary (argv[1], &sz);

    // write REX file
    struct rex_header *header = rex_header_create();
    struct rex_unitypackage unity;
    unity.target_platform = 2; // only WSA is currently supported
    unity.unity_version = unity_version;
    unity.data = buf;
    unity.sz = sz;
    long unity_sz;
    uint8_t *unity_ptr = rex_block_write_unitypackage(0 /*id*/, header, &unity, &unity_sz);

    long header_sz;
    uint8_t *header_ptr = rex_header_write(header, &header_sz);

    fwrite(header_ptr, header_sz, 1, fp);
    fwrite(unity_ptr, unity_sz, 1, fp);
    fclose(fp);

    FREE (buf);
    FREE (unity_ptr);
    FREE (header_ptr);
    return 0;
}
