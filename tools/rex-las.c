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
 * This file generates a REX file (pointlist) out of a given LAS file.
 * For LAS reading, the slash header file is used.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rex.h"
#include "slash.h"

#define MIN_VAL (0.0f)
#define MAX_VAL (1000.0f)

/* Current limitation for point list */
#define MAX_POINTS (100000)

void usage (const char *exec)
{
    die ("usage: %s lasfile rexfile\n", exec);
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n");
    printf ("Generating REX file from LAS file ...\n\n");

    if (argc < 3)
        usage (argv[0]);

    LAS *las;
    las = las_open (argv[1], "rb");
    las_header_display (las, stdout);

    int c = 0;
    int max_points = fmin (MAX_POINTS, las->number_of_point_records);
    int i = 0;

    struct rex_header *header = rex_header_create();
    struct rex_pointlist pointlist;
    rex_pointlist_init (&pointlist);

    pointlist.nr_vertices = max_points;
    pointlist.nr_colors = max_points;
    pointlist.positions = malloc (12 * pointlist.nr_vertices);
    pointlist.colors = malloc (12 * pointlist.nr_colors);

    while (las_read (las))
    {
        double x, y, z;
        if (c++ > max_points - 1)
            break;
        x = las_x (las) - las->x_offset;
        y = las_y (las) - las->y_offset;
        z = las_z (las) - las->z_offset;
        LAS_NRGB col = las_colour (las);
        // x
        pointlist.positions[i] = x;
        pointlist.colors[i++] = col.r;
        // y
        pointlist.positions[i] = y;
        pointlist.colors[i++] = col.g;
        // z
        pointlist.positions[i] = z;
        pointlist.colors[i++] = col.b;
        /* printf ("%f %f %f - %f - %f %f %f\n", x, y, z, intensity, r, g, b); */
    }

    las_close (las);

    FILE *fp = fopen (argv[2], "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", argv[2]);

    long p_sz;
    uint8_t *p_ptr = rex_block_write_pointlist (0 /*id*/, header, &pointlist, &p_sz);

    printf ("\nSuccessfully converted %d points.\n", pointlist.nr_vertices);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    FREE (pointlist.positions);
    FREE (pointlist.colors);

    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (p_ptr, p_sz, 1, fp);
    fclose (fp);

    FREE (p_ptr);
    FREE (header_ptr);
    return 0;
}
