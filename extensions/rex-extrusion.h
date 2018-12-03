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

/**
 * @brief normal computed as cross product of u x v
 * Introduces floating point error at, at least, the 7th decimal.
 */
void compute_normal (float ux, float uy, float uz,
                     float vx, float vy, float vz,
                     float *nx, float *ny, float *nz);

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
                              float height, uint64_t material_id, char* name);

/**
 * @brief Creates a Rex pointlist given an array of 3d points
 */
struct rex_pointlist* create_anchors (float *anchorpoints, uint32_t numanchors);

/**
 * @brief Print the given mesh in OFF format on stdout
 *
 * @TODO make the rex -> off converter standalone
 */
void off_print(struct rex_mesh *mesh);

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
                                       struct rex_material_standard* material, FILE *fp);

/**
 * @brief rex_extruded_file_write
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
void rex_extruded_file_write(float* points, uint32_t numpoints, float height,
                        float* anchorpoints, uint32_t numanchors, char* name,
                        FILE *fp);

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
 * @param filename with path where the output file should be written to
 */
void rex_extruded_write(float* points, uint32_t numpoints, float height,
                        float* anchorpoints, uint32_t numanchors, char* name,
                        char* filename);
