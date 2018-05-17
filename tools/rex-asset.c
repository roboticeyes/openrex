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

    if (read_and_write_asset_file(fp, &header, argv[1], ".a_rexasset",0))
    {
        fclose (fp);
        return -1;
    }
        if (read_and_write_asset_file(fp, &header, argv[1], ".i_rexasset",1))
    {
        fclose (fp);
        return -1;
    }
        if (read_and_write_asset_file(fp, &header, argv[1], ".w_rexasset",2))
    {
        fclose (fp);
        return -1;
    }

    rex_write_header (fp, &header);

    fclose (fp);
    return 0;
}

int read_and_write_asset_file(FILE *fp, struct rex_header *header, char *fileName, char *fileEnding, uint64_t id)
{
    // write asset file
    char *fileNameWithEnding;
    fileNameWithEnding = malloc(strlen(fileName) + strlen(fileEnding) + 1);
    strcpy(fileNameWithEnding, fileName);
    strcat(fileNameWithEnding, fileEnding);
    

    FILE *assetFile = fopen (fileNameWithEnding, "rb");
    if (!assetFile)
        die ("Cannot open rex asset file %s\n", fileNameWithEnding);
    fseek (assetFile, 0, SEEK_END);
    uint64_t fileSize = ftell (assetFile);
    fseek (assetFile, 0, SEEK_SET);
    uint8_t *blob = malloc (fileSize);
    if (fread (blob, fileSize, 1, assetFile) != 1)
        die ("Cannot read rexasset content");
    fclose (assetFile);

    uint16_t targetPlatform;
    if(strcmp(fileEnding,".a_rexasset") == 0)
    {
        targetPlatform = TARGET_PLATFROM_ANDROID;
    }
    else if(strcmp(fileEnding,".i_rexasset") == 0)
    {
        targetPlatform = TARGET_PLATFROM_IOS;
    }
    else if(strcmp(fileEnding,".w_rexasset") == 0)
    {
        targetPlatform = TARGET_PLATFROM_WSA;
    }
    
    if (rex_write_rexasset_block (fp, header, blob, fileSize, fileNameWithEnding, targetPlatform, id))
    {
        warn ("Error during file write %d\n", errno);
        free (blob);
        fclose (fp);
        return -1;
    }
    free(fileNameWithEnding);
    fileNameWithEnding = NULL;
    free (blob);
    blob = NULL;
    return 0;
}
