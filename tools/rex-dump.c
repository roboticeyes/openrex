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
#include <stdlib.h>

#include "rex.h"

void usage (const char *exec)
{
    die ("usage: %s filename.rex block_id\n", exec);
}

int main (int argc, char **argv)
{
    if (argc < 3)
        usage (argv[0]);

    //Should we do proper parameters as input?
    //sanity check?
    int64_t requested_id = atoi (argv[2]);

    long sz;
    uint8_t *buf = read_file_binary (argv[1], &sz);
    if (buf == NULL)
        die ("Cannot open REX file %s\n", argv[1]);

    struct rex_header header;
    uint8_t *ptr = rex_header_read (buf, &header);
    if (ptr == NULL)
        die ("Cannot read REX header");

    for (int i = 0; i < header.nr_datablocks; i++)
    {
        //TODO move pointer without reading the block for speedup
        struct rex_block block;
        ptr = rex_block_read (ptr, &block);

        //dump selected block
        if (block.id == (uint64_t) requested_id)
        {
            if (block.type == Image)
            {
                struct rex_image *img = block.data;
                fwrite (img->data, sizeof (uint8_t), img->sz, stdout);
            }
        }

        //free allocated memory
        if (block.type == Image)
        {
            struct rex_image *img = block.data;
            FREE (img->data);
            FREE (block.data);
        }
        else if (block.type == LineSet)
        {
            struct rex_lineset *ls = block.data;
            FREE (ls->vertices);
            FREE (block.data);
        }
        else if (block.type == PointList)
        {
            struct rex_pointlist *p = block.data;
            FREE (p->positions);
            FREE (block.data);
        }
        else if (block.type == Text)
        {
            FREE (block.data);
        }
        else if (block.type == Mesh)
        {
            struct rex_mesh *mesh = block.data;
            rex_mesh_free (mesh);
            FREE (block.data);
        }
        else if (block.type == MaterialStandard)
        {
            FREE (block.data);
        }
    }
    FREE (buf);
    return 0;
}
