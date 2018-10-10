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
 * This file converts a GeoJSON file into a REX file.
 *
 * The GeoJSON specification can be found here:
 * https://tools.ietf.org/html/rfc7946
 *
 * Implementation status: the following types are supported
 *
 * - FeatureCollection
 *
 * A sample can be found in the data directory
 *
 * TODO: this is just a prototype implementation and required more work
 */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "rex.h"

// global values for handling the global offset
static double ofs_north, ofs_east;
static int ofs_set = 0;

void usage (const char *exec)
{
    die ("usage: %s geojson.json filename.rex\n", exec);
}

// WGS84 parameters
static double equatorial_radius = 6378137.0;
static double ecc_squared       = 0.0066943799901413165;
static double deg2rad          = M_PI / 180.0;
static double k0               = 0.9996;

// lat and long are in degrees;  North latitudes and East Longitudes are positive.
void LLtoUTM (double lat, double lon, double *northing, double *easting, int *zone)
{
    double a = equatorial_radius;
    double ee = ecc_squared;
    lon -= (int) ( (lon + 180) / 360.0) * 360.0; // ensure longitude within -180.00..179.9
    double N, T, C, A, M;
    double lat_rad = lat * deg2rad;
    double lon_rad = lon * deg2rad;

    *zone = (int) ( (lon + 186) / 6.0);
    if (lat >= 56.0 && lat < 64.0 && lon >= 3.0 && lon < 12.0)  *zone = 32;
    if (lat >= 72.0 && lat < 84.0)            // special zones for Svalbard
    {
        if (lon >= 0.0  && lon <  9.0)  *zone = 31;
        else if (lon >= 9.0  && lon < 21.0)  *zone = 33;
        else if (lon >= 21.0 && lon < 33.0)  *zone = 35;
        else if (lon >= 33.0 && lon < 42.0)  *zone = 37;
    }
    double lon_origin = *zone * 6 - 183;          // origin in middle of zone
    double lon_origin_rad = lon_origin * deg2rad;

    double EE = ee / (1 - ee);

    N = a / sqrt (1 - ee * sin (lat_rad) * sin (lat_rad));
    T = tan (lat_rad) * tan (lat_rad);
    C = EE * cos (lat_rad) * cos (lat_rad);
    A = cos (lat_rad) * (lon_rad - lon_origin_rad);

    M = a * ( (1 - ee / 4    - 3 * ee * ee / 64 - 5 * ee * ee * ee / 256) * lat_rad
              - (3 * ee / 8 + 3 * ee * ee / 32 + 45 * ee * ee * ee / 1024) * sin (2 * lat_rad)
              + (15 * ee * ee / 256 + 45 * ee * ee * ee / 1024) * sin (4 * lat_rad)
              - (35 * ee * ee * ee / 3072) * sin (6 * lat_rad));

    *easting = k0 * N * (A + (1 - T + C) * A * A * A / 6 + (5 - 18 * T + T * T + 72 * C - 58 * EE) * A * A * A * A * A / 120) + 500000.0;

    *northing = k0 * (M + N * tan (lat_rad) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24
                      + (61 - 58 * T + T * T + 600 * C - 330 * EE) * A * A * A * A * A * A / 720));
}

void geojson_read_feature (const cJSON *f, struct rex_lineset *ls)
{
    cJSON *geom = cJSON_GetObjectItemCaseSensitive (f, "geometry");
    cJSON *type = cJSON_GetObjectItemCaseSensitive (geom, "type");

    if (cJSON_IsString (type) && strcmp (type->valuestring, "Polygon") == 0)
    {
        cJSON *coords = cJSON_GetObjectItemCaseSensitive (geom, "coordinates");
        cJSON *pts = coords->child;
        cJSON *pt = NULL;

        // generate a lineset for each polygon
        ls->nr_vertices = cJSON_GetArraySize (pts);
        ls->vertices = malloc (ls->nr_vertices * 12);
        int c = 0;
        cJSON_ArrayForEach (pt, pts)
        {
            cJSON *pt_x = cJSON_GetArrayItem (pt, 0);
            cJSON *pt_y = cJSON_GetArrayItem (pt, 1);
            cJSON *pt_z = cJSON_GetArrayItem (pt, 2);

            double x, y, z;
            x = pt_x->valuedouble;
            y = pt_y->valuedouble;
            if (pt_z == NULL)
                z = 0.0;
            else
                z = pt_z->valuedouble;

            double north, east;
            int zone;
            LLtoUTM (y, x, &north, &east, &zone);

            // set the offset to the first point
            if (!ofs_set)
            {
                ofs_north = north;
                ofs_east = east;
                ofs_set = 1;
            }

            north -= ofs_north;
            east  -= ofs_east;
            double scale = 1.0f; // can be used to scale down
            ls->vertices[c++] = north * scale;
            ls->vertices[c++] = z;
            ls->vertices[c++] = east * scale;
            printf ("coord [%f, %f, %f]\n", north * scale, east * scale, z);
        }
    }
    printf ("\n");
}

void geojson_convert (const char *filename, const char *rexfile)
{
    // Read input GeoJson file
    FILE *fp = NULL;
    char *content = read_file_ascii (filename);
    if (content == NULL)
        die ("Cannot open GeoJson file %s\n", filename);

    cJSON *json = cJSON_Parse (content);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf (stderr, "Error before: %s\n", error_ptr);
            cJSON_Delete (json);
            FREE (content);
            return;
        }
    }

    // Prepare REX output file
    struct rex_header *header = rex_header_create();
    fp = fopen (rexfile, "wb");
    if (!fp)
        die ("Cannot open REX file %s for writing\n", rexfile);

    // Write dummy header
    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);
    fwrite (header_ptr, header_sz, 1, fp);
    FREE (header_ptr);

    // Start parsing/conversion
    const cJSON *type = cJSON_GetObjectItemCaseSensitive (json, "type");
    if (cJSON_IsString (type) && (type->valuestring != NULL))
        printf ("Found supported type\"%s\"\n", type->valuestring);
    else
    {
        printf ("Found not supported type\"%s\"\n", type->valuestring);
        goto end;
    }

    const cJSON *features = cJSON_GetObjectItemCaseSensitive (json, "features");
    const cJSON *f = NULL;
    uint64_t id = 0;

    cJSON_ArrayForEach (f, features)
    {
        cJSON *type = cJSON_GetObjectItemCaseSensitive (f, "type");
        if (strcmp (type->valuestring, "Feature") == 0)
        {
            struct rex_lineset ls;
            ls.red = 0.9f;
            ls.green = 0.0f;
            ls.blue = 0.0f;
            ls.alpha = 0.8f;
            geojson_read_feature (f, &ls);

            long ls_sz;
            uint8_t *ls_ptr = rex_block_write_lineset (id++, header, &ls, &ls_sz);
            fwrite (ls_ptr, ls_sz, 1, fp);
            FREE (ls_ptr);
            FREE (ls.vertices);
        }
        else
            printf ("Not supported type: %s\n", type->valuestring);
    }

    // Write correct header
    fseek (fp, 0, SEEK_SET);
    header_ptr = rex_header_write (header, &header_sz);
    fwrite (header_ptr, header_sz, 1, fp);
    FREE (header_ptr);

end:
    cJSON_Delete (json);
    FREE (content);
    FREE (header);
    if (fp != NULL)
        fclose (fp);
}
int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n");
    printf ("Converting GeoJSON to REX file ...\n\n");

    if (argc < 3)
        usage (argv[0]);

    // convert GeoJSON file
    geojson_convert (argv[1], argv[2]);
    return 0;
}
