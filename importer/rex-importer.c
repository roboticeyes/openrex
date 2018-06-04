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
#include "rex.h"

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/postprocess.h>    // Post processing flags
#include <assimp/scene.h>          // Output data structure
#include <string.h>

void usage (const char *exec)
{
    die ("usage: %s inputfile outputfile\n", exec);
}

void convert_mesh (struct aiMesh *input, struct rex_mesh *mesh)
{
    int i, j;
    printf ("Mesh:     %s\n", input->mName.data);
    printf ("Faces:    %d\n", input->mNumFaces);
    printf ("Vertices: %d\n", input->mNumVertices);

    if (!mesh) die ("Mesh point failure");
    if (input->mNumFaces == 0)
    {
        warn ("No triangles found!");
        return;
    }

    mesh->lod = mesh->max_lod = 0;
    snprintf (mesh->name, REX_MESH_NAME_MAX_SIZE - 1, "%s", input->mName.data);
    mesh->material_id = 0; // TODO .. currently only using default material
    mesh->nr_vertices = input->mNumVertices;
    mesh->nr_triangles = input->mNumFaces;

    mesh->positions = malloc (sizeof (float) * 3 * mesh->nr_vertices);
    for (i = 0, j = 0; j < input->mNumVertices; i += 3, j++)
    {
        // transform input (Z is up)
        mat4x4 mat =
        {
            {1,  0,  0,  0},
            {0,  0,  1,  0},
            {0,  1,  0,  0},
            {0,  0,  0,  1}
        };
        vec4 r;
        vec4 v = {input->mVertices[j].x, input->mVertices[j].y, input->mVertices[j].z, 1.0f };
        mat4x4_mul_vec4 (r, mat, v);

        mesh->positions[i]     = r[0];
        mesh->positions[i + 1] = r[1];
        mesh->positions[i + 2] = r[2];
        /* printf ("p %5d: %5.2f %5.2f %5.2f\n", j, */
        /*         mesh->positions[i], */
        /*         mesh->positions[i + 1], */
        /*         mesh->positions[i + 2]); */
    }

    if (input->mNormals != NULL)
    {
        mesh->normals = malloc (sizeof (float) * 3 * mesh->nr_vertices);
        for (i = 0, j = 0; j < input->mNumVertices; i += 3, j++)
        {
            mesh->normals[i]     = input->mNormals[j].x;
            mesh->normals[i + 1] = input->mNormals[j].y;
            mesh->normals[i + 2] = input->mNormals[j].z;
        }
    }
#if 0
    if (input->mTextureCoords[0] != NULL)
    {
        mesh->tex_coords = malloc (sizeof (float) * 2 * mesh->nr_vertices);
        for (i = 0, j = 0; j < input->mNumVertices; i += 2, j++)
        {
            mesh->tex_coords[i] = input->mTextureCoords[0][j].x;
            mesh->tex_coords[i + 1] = input->mTextureCoords[0][j].y;
        }
    }
#endif
    // TODO currently not supported
    /* mesh->colors = malloc (sizeof (float) * 3 * mesh->nr_vertices); */

    mesh->triangles = malloc (sizeof (uint32_t) * 3 * mesh->nr_triangles);
    for (i = 0, j = 0; j < input->mNumFaces; i += 3, j++)
    {
        mesh->triangles[i]     = input->mFaces[j].mIndices[0];
        mesh->triangles[i + 1] = input->mFaces[j].mIndices[1];
        mesh->triangles[i + 2] = input->mFaces[j].mIndices[2];
        /* printf ("f %5d: %5d %5d %5d\n", j, */
        /*         mesh->triangles[i], */
        /*         mesh->triangles[i + 1], */
        /*         mesh->triangles[i + 2]); */
    }
}

void process_material (struct aiMaterial *mat)
{
    struct aiString name;
    aiGetMaterialString (mat, AI_MATKEY_NAME, &name);
    printf ("Material Name:     %s\n", name.data);

#if 0
    int max = 1;
    float x;
    if (aiGetMaterialFloatArray (mat, AI_MATKEY_SHININESS, &x, &max) == AI_SUCCESS)
        model->mat.specularPower = x;
    if (aiGetMaterialFloatArray (mat, AI_MATKEY_SHININESS_STRENGTH, &x, &max) == AI_SUCCESS)
        model->mat.specularIntensity = x;

    printf ("POWER: %.2f, INTENSITY %.2f\n", model->mat.specularPower, model->mat.specularIntensity);

    struct aiString path;

    SDL_Surface *image;
    if (texturePath != NULL)
        mesh_textureFromFile (model, texturePath);
    else if (aiGetMaterialString (mat, AI_MATKEY_TEXTURE (aiTextureType_DIFFUSE, 0), &path) == AI_SUCCESS)
    {
        char filename[9 + path.length];
        char r[] = "res/obj/";

        int i;
        for (i = 0; i < 8 + path.length; i++)
        {
            if (i < 8)
                filename[i] = r[i];
            else
                filename[i] = path.data[i - 8];
        }
        filename[i] = 0;

        mesh_textureFromFile (model, texturePath);
    }
    else
        return;
#endif
}

int main (int argc, char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    if (argc < 3)
        usage (argv[0]);

    /* Import Assimp file and perform post-triangulation */
    /* const struct aiScene *scene = aiImportFile (argv[1], aiProcess_Triangulate); */
    const struct aiScene *scene = aiImportFile (argv[1],
            aiProcessPreset_TargetRealtime_Quality
                                  /* aiProcess_JoinIdenticalVertices | */
                                  /* aiProcess_Triangulate | */
                                  /* aiProcess_SortByPType | */
                                  /* aiProcess_GenNormals */
                                               );
    if (!scene)
        die ("Cannot import scene: %s", aiGetErrorString());

    printf ("Found %d scenes in input file.\n", scene->mNumMeshes);

    struct rex_header *header = rex_header_create();

    // write a default material
    struct rex_material_standard mat =
    {
        .ka_red = 0,
        .ka_green = 0,
        .ka_blue = 0,
        .ka_textureId = REX_NOT_SET,
        .kd_red = 1,
        .kd_green = 0,
        .kd_blue = 0,
        .kd_textureId = REX_NOT_SET,
        .ks_red = 0,
        .ks_green = 0,
        .ks_blue = 0,
        .ks_textureId = REX_NOT_SET,
        .ns = 0,
        .alpha = 1
    };
    long mat_sz;
    uint8_t *mat_ptr = rex_block_write_material (0 /*id*/, header, &mat, &mat_sz);

    int i;
    uint8_t *mesh_data[scene->mNumMeshes];
    long mesh_block_sizes[scene->mNumMeshes];

    for (i = 0; i < scene->mNumMeshes; i++)
    {
        struct rex_mesh rex_mesh;
        rex_mesh_init (&rex_mesh);
        process_material (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
        convert_mesh (scene->mMeshes[i], &rex_mesh);
        uint8_t *mesh_ptr = rex_block_write_mesh (i + 1 /*id*/, header, &rex_mesh, &mesh_block_sizes[i]);
        rex_mesh_free (&rex_mesh);
        mesh_data[i] = mesh_ptr;

        /* mesh_setData (scene->mMeshes[i], model); */
        /* mesh_setMaterialData (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], model, texturePath); */
        /* list_insert (meshList, model); */
    }

    // write header blob
    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    // write data to file
    FILE *fp = fopen (argv[2], "wb");
    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (mat_ptr, mat_sz, 1, fp);

    // write all mesh data
    for (i = 0; i < scene->mNumMeshes; i++)
        fwrite (mesh_data[i], mesh_block_sizes[i], 1, fp);
    fclose (fp);
    return 0;
}
