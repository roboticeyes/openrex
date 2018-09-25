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
#include <math.h>

#include "rex.h"

void usage (const char *exec)
{
    die ("usage: %s outputfilename.rex [pointdata.csv] [anchordata.csv]\n\
          .csv with a header line with number of points (delimiter can be space or comma)\n", exec);
}

/**
 * @brief normal computed as cross product of u x v
 * Introduces floating point error at, at least, the 7th decimal.
 */
void compute_normal (float ux, float uy, float uz,
                     float vx, float vy, float vz,
                     float *nx, float *ny, float *nz)
{
    *nx = uy*vz - uz*vy;
    *ny = uz*vx - ux*vz;
    *nz = ux*vy - uy*vx;

    //normalize
    float normsq = *nx * *nx + *ny * *ny + *nz * *nz;

    //floating point error at the 7th decimal at least
    float norm = sqrtf (normsq);
    *nx = *nx/norm;
    *ny = *ny/norm;
    *nz = *nz/norm;
}

/**
 * @brief Build a mesh given a coplanar array of 3d points.
 * The points are extruded in the normal direction,
 * using right-hand rule to choose which of the two normals.
 * The material_id can be REX_NOT_SET.
 *
 * @points array of coplanar 3d points, in the right order.
 * Repeat the first point for closed polygons
 * @numpoints the number of 3d points, thus numpoints=length(points)/3
 * @height length of the extrusion vector
 * @material_id id of the material to use. The material has to be present in the rex file
 * @name char array with the desidred name of the mesh.
 */
struct rex_mesh* rex_extrude (float *points, uint32_t numpoints,
                              float height, uint64_t material_id, char* name)
{
    struct rex_mesh *mesh = malloc (sizeof (struct rex_mesh));
    if (!mesh)
        die ("Allocating mesh failed");
    rex_mesh_init (mesh);

    int closed = 0;
    if (fabsf (points[0] - points[3 * (numpoints - 1)])     < REX_EPSILON_FLOAT
     && fabsf (points[1] - points[3 * (numpoints - 1) + 1]) < REX_EPSILON_FLOAT
     && fabsf (points[2] - points[3 * (numpoints - 1) + 2]) < REX_EPSILON_FLOAT)
        closed = 1;

    if (closed == 1) //remove the last repeated point now that we've flagged that it's closed
        numpoints = numpoints - 1;

    mesh->material_id  = material_id;
    mesh->nr_vertices  = 2 * numpoints;
    mesh->nr_triangles = 2 * 2 * numpoints; //2x because both orientations

    //eew and not null-terminated if overflow
    strncpy (mesh->name, name, strlen (name) + 1 < REX_MESH_NAME_MAX_SIZE? strlen (name) + 1 : REX_MESH_NAME_MAX_SIZE);

    mesh->positions = malloc (3 * mesh->nr_vertices * sizeof (float));
    if (!mesh->positions)
        die ("Allocating extruded mesh failed");

    memcpy (mesh->positions, points, 3 * numpoints * sizeof (float));

    mesh->triangles = malloc (3 * mesh->nr_triangles * sizeof (uint32_t));
    if (!mesh->triangles)
        die ("Allocating extruded mesh failed");

    uint32_t triangle_vertex_id = 0;

    if (numpoints < 3)
        die ("Less than three points is not supported, since we can't compute the normal.");

    //get the plane vectors (the direction of the normal is defined by
    //the order of the input points and right-hand rule, thus,
    //clockwise down, counterclockwise up
    float ux = mesh->positions[6] - mesh->positions[3];
    float uy = mesh->positions[7] - mesh->positions[4];
    float uz = mesh->positions[8] - mesh->positions[5];
    float vx = mesh->positions[0] - mesh->positions[3];
    float vy = mesh->positions[1] - mesh->positions[4];
    float vz = mesh->positions[2] - mesh->positions[5];

    float nx, ny, nz;

    compute_normal (ux, uy, uz, vx, vy, vz, &nx, &ny, &nz);

    //difference vectors to use as extrusion
    float dx = height * nx;
    float dy = height * ny;
    float dz = height * nz;

    //traverse the original array of points
    for (unsigned int i = 0; i < numpoints; i++)
    {
        float x = points[3 * i];
        float y = points[3 * i + 1];
        float z = points[3 * i + 2];

        //set vertex coordinates
        mesh->positions[3 * numpoints + 3 * i]     = x + dx;
        mesh->positions[3 * numpoints + 3 * i + 1] = y + dy;
        mesh->positions[3 * numpoints + 3 * i + 2] = z + dz;

        //set normals coordinates (optional)

        //set textures coordinates (optional)

        //set colors block (optional)

        //each point creates four triangles, two for each orientation

        if (closed == 0 && i == numpoints - 1) //if not closed, skip last point's triangles
            break;

        //upside down interior next triangle
        mesh->triangles[triangle_vertex_id] = i; //current point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = i + numpoints; //current extruded point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = (i+1 + numpoints)%numpoints + numpoints; //next extruded point
        triangle_vertex_id++;

        //downside up interior next triangle
        mesh->triangles[triangle_vertex_id] = i; //current point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = (i+1 + numpoints)%numpoints + numpoints; //next extruded point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = (i+1)%numpoints; //next point
        triangle_vertex_id++;

        //upside down exterior next triangle
        mesh->triangles[triangle_vertex_id] = i; //current point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = (i+1 + numpoints)%numpoints + numpoints; //next extruded point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = i + numpoints; //current extruded point
        triangle_vertex_id++;

        //downside up exterior next triangle
        mesh->triangles[triangle_vertex_id] = i; //current point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = (i+1)%numpoints; //next point
        triangle_vertex_id++;
        mesh->triangles[triangle_vertex_id] = (i+1 + numpoints)%numpoints + numpoints; //next extruded point
        triangle_vertex_id++;
    }

    return mesh;
}

/**
 * @brief Creates a Rex pointlist given an array of 3d points
 */
struct rex_pointlist* create_anchors (float *anchorpoints, uint32_t numanchors)
{    
    struct rex_pointlist* pointlist = malloc (sizeof (struct rex_pointlist));
    if (!pointlist)
        die ("Allocating pointlist failed");
    rex_pointlist_init (pointlist);

    pointlist->nr_vertices = numanchors;
    pointlist->nr_colors = 0;

    pointlist->positions = malloc (3 * numanchors * sizeof (float));
    memcpy (pointlist->positions, anchorpoints, 3 * numanchors * sizeof (float));

    return pointlist;
}

/**
 * @brief Print the given mesh in OFF format on stdout
 *
 * @TODO make the rex -> off converter standalone
 */
void off_print(struct rex_mesh *mesh)
{
    //off format for debugging
    printf ("\nOFF\n");
    printf ("%d %d %d\n", mesh->nr_vertices, mesh->nr_triangles, mesh->nr_triangles*3);
    for (uint32_t i = 0; i < 3 * mesh->nr_vertices; i = i + 3)
    {
        printf ("%.20f %.20f %.20f\n", mesh->positions[i], mesh->positions[i + 1], mesh->positions[i + 2]);
    }
    for (uint32_t i = 0; i < 3 * mesh->nr_triangles; i = i + 3)
    {
        printf ("3 %d %d %d\n", mesh->triangles[i], mesh->triangles[i + 1], mesh->triangles[i + 2]);
    }
}

/**
 * @brief rex_extruded_write
 * Writes a rex_file with a extruded mesh, the anchor pointlist (if any),
 * and the given rex material as mesh material
 * @param points array of coplanar 3d points
 * @param numpoints number of coplanar 3d points
 * @param height lenght of the desired extrusion vector
 * @param anchorpoints array of 3d points
 * @param numanchors number of 3d anchorpoints
 * @param name name of the mesh
 * @param fp output file to write to
 */
void rex_extruded_with_material_write (float* points, uint32_t numpoints, float height,
                                       float* anchorpoints, uint32_t numanchors, char* name,
                                       struct rex_material_standard* material, FILE *fp)
{
    struct rex_header *header = rex_header_create();

    uint64_t material_id = REX_NOT_SET;

    struct rex_mesh* mesh;
    mesh = rex_extrude (points, numpoints, height, material_id, name);


    struct rex_pointlist* pointlist = NULL;
    if (numanchors > 0)
        pointlist = create_anchors (anchorpoints, numanchors);

    //write to rex file
    long mesh_sz;
    uint8_t *mesh_ptr = rex_block_write_mesh (0 /*id*/, header, mesh, &mesh_sz);

    long pointlist_sz = 0;
    uint8_t *pointlist_ptr = NULL;
    if (numanchors > 0)
        pointlist_ptr = rex_block_write_pointlist (1 /*id*/, header, pointlist, &pointlist_sz);

    long material_sz = 0;
    uint8_t *material_ptr = NULL;
    if (material != NULL)
        material_ptr = rex_block_write_material (material_id, header, material, &material_sz);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    fwrite (header_ptr, (size_t) header_sz, 1, fp);

    if (numanchors > 0)
        fwrite (pointlist_ptr, (size_t) pointlist_sz, 1, fp);

    fwrite (mesh_ptr, (size_t) mesh_sz, 1, fp);

    if (material != NULL)
        fwrite (material_ptr, (size_t) material_sz, 1, fp);

    fclose (fp);

    //cleanup
    rex_mesh_free (mesh);
    if (numanchors > 0)
        rex_pointlist_free (pointlist);
    FREE (material_ptr);
    FREE (mesh_ptr);
    FREE (pointlist_ptr);
    FREE (header_ptr);
}

/**
 * @brief rex_extruded_write
 * Writes a rex_file with a extruded mesh, the anchor pointlist (if any),
 * and a orange color as material
 * @param points array of coplanar 3d points
 * @param numpoints number of coplanar 3d points
 * @param height lenght of the desired extrusion vector
 * @param anchorpoints array of 3d points
 * @param numanchors number of 3d anchorpoints
 * @param name name of the mesh
 * @param fp output file to write to
 */
void rex_extruded_write(float* points, uint32_t numpoints, float height,
                        float* anchorpoints, uint32_t numanchors, char* name,
                        FILE *fp)
{
    //FIXME this values give me the ring of death
    struct rex_material_standard material =
    {
        .ka_red   = 0.95f,
        .ka_green = 0.58f,
        .ka_blue  = 0,
        .ka_textureId = REX_NOT_SET,
        .kd_red   = 1,
        .kd_green = 0.50f,
        .kd_blue  = 0,
        .kd_textureId = REX_NOT_SET,
        .ks_red   = 0.0225f,
        .ks_green = 0.0225f,
        .ks_blue  = 0.0225f,
        .ks_textureId = REX_NOT_SET,
        .ns = 0,
        .alpha = 1
    };

    rex_extruded_with_material_write (points, numpoints, height,
                                      anchorpoints, numanchors, name,
                                      &material, fp);
}

/**
 * @brief read_csv_array reads an array from file, either comma-separated or space separater
 * @param filename filename of the file to read
 * @param numelements returns the number of elements read
 * @return array of floats read
 */
float* read_csv_array (char *filename, unsigned int* numelements)
{
    FILE* fd = fopen (filename, "r");

    unsigned int num_read_elements = 0;

    char line[1024];
    if (fgets (line, 1024, fd))
    {
        for (const char* tok = strtok (line, " ,\n"); tok && *tok; tok = strtok (NULL, " ,\n"))
        {
            unsigned int elements = (unsigned int) atoi (tok);
            num_read_elements += 3 * elements;
        }
    }

    if (num_read_elements == 0)
        die("error reading %s\n", filename);

    float* array = malloc (num_read_elements * sizeof (float));

    unsigned int i = 0;

    while (fgets (line, 1024, fd))
    {
        for (const char* tok = strtok (line, " ,\n"); tok && *tok; tok = strtok (NULL, " ,\n"))
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

    float* points                    = NULL;
    uint32_t points_array_size       = 0;
    float* anchorpoints              = NULL;
    uint32_t anchorpoints_array_size = 0;

    char* polygonfilename = NULL;
    char* anchorsfilename = NULL;
    char* outputfilename  = "extruded.rex";

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
                                                      1, 3, 2};
        points_array_size = sizeof (points_default) / sizeof (points_default[0]);
        static const float anchorpoints_default[3 * 4] = {1.5,   1,   2,
                                                         1.33f, 2.5, 2.5,
                                                         1.25,   3,   2,
                                                          3.5,   1, 2.5};
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
        die ("Cannot open REX file %s for writing\n", argv[1]);

    float height         = 3;
    uint32_t numpoints   = points_array_size / 3;
    uint32_t numanchors  = anchorpoints_array_size / 3;
    char* name           = "Robotic Eyes Extruded Mesh";

    rex_extruded_write (points, numpoints, height, anchorpoints, numanchors, name, fp);

    printf ("[REX extruder] Output written at %s\n", outputfilename);

    FREE (points);
    FREE (anchorpoints);

    printf ("\n[REX extruder] Job's Done, Sir. Have a Nice Day.\n");

    return 0;
}
