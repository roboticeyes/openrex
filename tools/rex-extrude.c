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

#include "rex-extrusion.h"
#include "rex.h"

void usage (const char *exec)
{
    die ("usage: %s outputfilename.rex [pointdata.csv] [anchordata.csv]\n\
          .csv with a header line with number of points (delimiter can be space or comma)\n", exec);
}

/**
 * @brief read_csv_array reads an array from file, either comma-separated or space separater
 * @param filename filename of the file to read
 * @param numelements returns the number of elements read
 * @return array of floats read
 */
float *read_csv_array (char *filename, unsigned int *numelements)
{
    FILE *fd = fopen (filename, "r");

    unsigned int num_read_elements = 0;

    char line[1024];
    if (fgets (line, 1024, fd))
    {
        for (const char *tok = strtok (line, " ,\n"); tok && *tok; tok = strtok (NULL, " ,\n"))
        {
            unsigned int elements = (unsigned int) atoi (tok);
            num_read_elements += 3 * elements;
        }
    }

    if (num_read_elements == 0)
        die ("error reading %s\n", filename);

    float *array = malloc (num_read_elements * sizeof (float));

    unsigned int i = 0;

    while (fgets (line, 1024, fd))
    {
        for (const char *tok = strtok (line, " ,\n"); tok && *tok; tok = strtok (NULL, " ,\n"))
        {
            if (i >= num_read_elements)
                die ("Number of elements read > number of points stated in the file header\n");
            array[i] = atof (tok);
            i++;
        }
    }

    fclose (fd);

    if (i != num_read_elements)
        die ("Number of elements read != number of points stated in the file header\n");

    *numelements = num_read_elements;
    return array;
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 2 || 4 < argc)
        usage (argv[0]);

    float *points                    = NULL;
    uint32_t points_array_size       = 0;
    float *anchorpoints              = NULL;
    uint32_t anchorpoints_array_size = 0;

    char *polygonfilename = NULL;
    char *anchorsfilename = NULL;
    char *outputfilename  = "extruded.rex";

    //TODO check file extension
    //    if (argv[1])
    //        die ("Output filename does not have rex extension. e.g. dummy.rex");

    if (argc == 3 || argc == 4) //points given
    {
        polygonfilename = argv[2];
        outputfilename  = argv[1];

        points = read_csv_array (polygonfilename, &points_array_size);
        if (points_array_size < 4)
            die ("%d values where read at %s. Minimum is 4 values.\n", points_array_size, polygonfilename);

        printf ("[REX extruder] %d points read from %s\n", points_array_size, polygonfilename);

        if (argc == 4)
        {
            anchorsfilename = argv[3];
            anchorpoints = read_csv_array (anchorsfilename, &anchorpoints_array_size);
            printf ("[REX extruder] %d anchor points read from %s\n", anchorpoints_array_size, anchorsfilename);
        }
    }
    else //default execution
    {
        if (argc == 2)
            outputfilename = argv[1];

        static const float points_default[3 * 6] = {  1, 1, 2,
                                                      4, 1, 0,
                                                      4, 2, 0,
                                                      2.5, 2, 1,
                                                      2.5, 3, 1,
                                                      1, 3, 2
                                                   };
        points_array_size = sizeof (points_default) / sizeof (points_default[0]);
        static const float anchorpoints_default[3 * 4] = {1.5,   1,   2,
                                                          1.33f, 2.5, 2.5,
                                                          1.25,   3,   2,
                                                          3.5,   1, 2.5
                                                         };
        anchorpoints_array_size = sizeof (anchorpoints_default) / sizeof (anchorpoints_default[0]);

        points = malloc (points_array_size * sizeof (float));
        if (!points)
            die ("Allocating default points memory failed");

        anchorpoints = malloc (anchorpoints_array_size * sizeof (float));
        if (!anchorpoints)
            die ("Allocating default anchorpoints memory failed");

        memcpy (points, points_default, points_array_size * sizeof (float));
        memcpy (anchorpoints, anchorpoints_default, anchorpoints_array_size * sizeof (float));
    }

    FILE *fp = fopen (outputfilename, "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", outputfilename);

    float height         = 3;
    uint32_t numpoints   = points_array_size / 3;
    uint32_t numanchors  = anchorpoints_array_size / 3;
    char *name           = "Robotic Eyes Extruded Mesh";

    rex_extruded_file_write (points, numpoints, height, anchorpoints, numanchors, name, fp);

    printf ("[REX extruder] Output written at %s\n", outputfilename);

    FREE (points);
    FREE (anchorpoints);

    printf ("\n[REX extruder] Job's Done, Sir. Have a Nice Day.\n");

    return 0;
}
