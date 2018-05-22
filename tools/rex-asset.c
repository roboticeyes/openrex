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
 * This tool takes a REX Unity Asset file for each target platform and generates a REX file.
 * The file endings have to be:
 *   ".a_rexasset" for Android
 *   ".i_rexasset" for iOS
 *   ".w_rexasset" for WSA
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "core.h"
#include "global.h"
#include "util.h"

#define TARGET_PLATFROM_ANDROID 0
#define TARGET_PLATFROM_IOS 1
#define TARGET_PLATFROM_WSA 2

void usage (const char *exec)
{
    die ("usage: %s <rex_asset_file_name_without_ending> filename.rex\n", exec);
}

int write_asset_package_to_rex_file(FILE *fp, struct rex_header *header, char *file_name, char *file_ending, uint64_t id)
{
    // write asset file
    char *file_name_with_ending;
    file_name_with_ending = malloc(strlen(file_name) + strlen(file_ending) + 1);
    strcpy(file_name_with_ending, file_name);
    strcat(file_name_with_ending, file_ending);

    FILE *asset_file = fopen (file_name_with_ending, "rb");
    if (!asset_file)
        die ("Cannot open rex asset file %s\n", file_name_with_ending);
    fseek (asset_file, 0, SEEK_END);
    uint64_t file_size = ftell (asset_file);
    fseek (asset_file, 0, SEEK_SET);
    uint8_t *blob = malloc (file_size);
    if (fread (blob, file_size, 1, asset_file) != 1)
        die ("Cannot read content of %s\n", file_name_with_ending);
    fclose (asset_file);

    uint16_t target_platform = 0;
    if(strcmp(file_ending,".a_rexasset") == 0)
        target_platform = TARGET_PLATFROM_ANDROID;
    else if(strcmp(file_ending,".i_rexasset") == 0)
        target_platform = TARGET_PLATFROM_IOS;
    else if(strcmp(file_ending,".w_rexasset") == 0)
        target_platform = TARGET_PLATFROM_WSA;
    
    if (rex_write_rexasset_block (fp, header, blob, file_size, target_platform, id))
    {
        warn ("Error during file write %d\n", errno);
        free (blob);
        blob = NULL;
        free(file_name_with_ending);
        file_name_with_ending = NULL;
        fclose (fp);
        return -1;
    }
    free(file_name_with_ending);
    file_name_with_ending = NULL;
    free (blob);
    blob = NULL;
    return 0;
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

    char *file_ending = ".a_rexasset";
    if (write_asset_package_to_rex_file(fp, &header, argv[1], file_ending, 0))
    {
        fclose (fp);
        die ("Could not write asset package %s%s to REX file\n", argv[1], file_ending);
    }
    file_ending = ".i_rexasset";
    if (write_asset_package_to_rex_file(fp, &header, argv[1], file_ending, 1))
    {
        fclose (fp);
        die ("Could not write asset package %s%s to REX file\n", argv[1], file_ending);
    }
    file_ending = ".w_rexasset";
    if (write_asset_package_to_rex_file(fp, &header, argv[1], file_ending, 2))
    {
        fclose (fp);
        die ("Could not write asset package %s%s to REX file\n", argv[1], file_ending);
    }

    rex_write_header (fp, &header);

    fclose (fp);
    return 0;
}

