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

#include "config.h"

#include "global.h"
#include "rex_core.h"
#include "util.h"

void usage (const char *exec)
{
    die ("usage: %s <rex_asset_file> filename.rex\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 3)
        usage (argv[0]);

    FILE *fp = fopen (argv[2], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[2]);

    struct rex_header header = rex_create_header();
    rex_write_header (fp, &header);

    // write texture file
    FILE *t = fopen (argv[1], "rb");
    if (!t)
        die ("Cannot open rex asset file %s\n", argv[1]);
    fseek (t, 0, SEEK_END);
    uint64_t sz = ftell (t);
    fseek (t, 0, SEEK_SET);
    uint8_t *blob = malloc (sz);
    if (fread (blob, sz, 1, t) != 1)
        die ("Cannot read rexasset content");
    fclose (t);

    if (rex_write_rexasset_bock (fp, &header, blob, sz, "rexasset", 0))
    {
        warn ("Error during file write %d\n", errno);
        free (blob);
        fclose (fp);
        return -1;
    }

    rex_write_header (fp, &header);

    fclose (fp);
    free (blob);
    return 0;
}

