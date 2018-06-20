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
 * This tool uses Assimp to convert geometry into the REX file format
 * This is a pre-liminary work, assimp does not properly support some formats!
 */
#include "rex.h"
#include "argparse.h"

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/postprocess.h>    // Post processing flags
#include <assimp/scene.h>          // Output data structure
#include <stdbool.h>
#include <string.h>

struct settings_s
{
    char *input;
    char *output;
    bool transform;
    float scale;
};

struct settings_s settings =
{
    .input = NULL,
    .output = NULL,
    .transform = true,
    .scale = 1.0f
};

struct argparse_option options[] =
{
    OPT_HELP(),
    OPT_GROUP ("Basic flags"),
    OPT_STRING ('i', "input", &settings.input, "input file"),
    OPT_STRING ('o', "output", &settings.output, "output file"),
    OPT_GROUP ("Geometric transformations"),
    OPT_BOOLEAN ('\0', "transform", &settings.transform, "apply transformation to have Z pointing upwards [default=true], use no- prefix to disable"),
    OPT_FLOAT ('s', "scale", &settings.scale, "apply coordinate scale (e.g. if input is not in unit meters)"),
    OPT_END(),
};

static const char *const usage[] =
{
    "rex-importer [options] -i inputfile -o outputfile",
    NULL,
};

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
    mesh->nr_vertices = input->mNumVertices;
    mesh->nr_triangles = input->mNumFaces;

    mesh->positions = malloc (sizeof (float) * 3 * mesh->nr_vertices);

    mat4x4 mat =
    {
        {1,  0,  0,  0},
        {0,  0,  1,  0},
        {0,  1,  0,  0},
        {0,  0,  0,  1}
    };
    for (i = 0, j = 0; j < input->mNumVertices; i += 3, j++)
    {
        vec4 r;
        vec4 v =
        {
            input->mVertices[j].x * settings.scale,
            input->mVertices[j].y * settings.scale,
            input->mVertices[j].z * settings.scale,
            1.0f
        };
        if (settings.transform)
            vec4_dup (r, v);
        else
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
    if (input->mTextureCoords[0] != NULL)
    {
        mesh->tex_coords = malloc (sizeof (float) * 2 * mesh->nr_vertices);
        for (i = 0, j = 0; j < input->mNumVertices; i += 2, j++)
        {
            mesh->tex_coords[i] = input->mTextureCoords[0][j].x;
            mesh->tex_coords[i + 1] = input->mTextureCoords[0][j].y;
        }
    }

    // TODO currently not supported by assimp
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

void convert_material (struct aiMaterial *mat, struct rex_material_standard *rex_mat)
{
    struct aiString name;
    aiGetMaterialString (mat, AI_MATKEY_NAME, &name);
    printf ("Material Name:     %s\n", name.data);

    struct aiColor4D color;
    if (aiGetMaterialColor (mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
    {
        rex_mat->kd_red = color.r;
        rex_mat->kd_green = color.g;
        rex_mat->kd_blue = color.b;
    }

    if (aiGetMaterialColor (mat, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS)
    {
        rex_mat->ka_red = color.r;
        rex_mat->ka_green = color.g;
        rex_mat->ka_blue = color.b;
    }

    if (aiGetMaterialColor (mat, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS)
    {
        rex_mat->ks_red = color.r;
        rex_mat->ks_green = color.g;
        rex_mat->ks_blue = color.b;
    }

    unsigned int max = 1;
    float val;
    if (aiGetMaterialFloatArray (mat, AI_MATKEY_OPACITY, &val, &max) == AI_SUCCESS)
        rex_mat->alpha = val;
    else
        rex_mat->alpha = 1.0f;

    struct aiString path;
    if (aiGetMaterialString (mat, AI_MATKEY_TEXTURE (aiTextureType_DIFFUSE, 0), &path) == AI_SUCCESS)
        printf ("Found diffuse texture: %s\n", path.data);
    if (aiGetMaterialString (mat, AI_MATKEY_TEXTURE (aiTextureType_AMBIENT, 0), &path) == AI_SUCCESS)
        printf ("Found ambient texture: %s\n", path.data);
    if (aiGetMaterialString (mat, AI_MATKEY_TEXTURE (aiTextureType_SPECULAR, 0), &path) == AI_SUCCESS)
        printf ("Found specular texture: %s\n", path.data);

    // TODO textures needs to be read out
    rex_mat->ka_textureId = REX_NOT_SET;
    rex_mat->kd_textureId = REX_NOT_SET;
    rex_mat->ks_textureId = REX_NOT_SET;
    rex_mat->ns = 0;
}

int main (int argc, const char **argv)
{
    printf ("═══════════════════════════════════════════\n");
    printf ("        %s %s (c) Robotic Eyes\n", rex_name, VERSION);
    printf ("═══════════════════════════════════════════\n\n");

    struct argparse argparse;
    argparse_init (&argparse, options, usage, 0);
    argparse_describe (&argparse,
                       "\nConvert 3D geometry file using Assimp into a REX file.",
                       "\nThis importer uses Assimp to convert 3D geometry.\n");
    argc = argparse_parse (&argparse, argc, argv);

    if (settings.input == NULL || settings.output == NULL)
    {
        argparse_usage (&argparse);
        return 1;
    }
    /* Import Assimp file and perform post-triangulation */
    const struct aiScene *scene = aiImportFile (settings.input,
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

    int i;
    uint8_t *mesh_data[scene->mNumMeshes];
    long mesh_data_sz[scene->mNumMeshes];
    uint8_t *mat_data[scene->mNumMeshes];
    long mat_data_sz[scene->mNumMeshes];

    long block_id = 0;

    for (i = 0; i < scene->mNumMeshes; i++)
    {
        struct rex_mesh rex_mesh;
        struct rex_material_standard rex_mat;

        rex_mesh_init (&rex_mesh);

        convert_material (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], &rex_mat);
        convert_mesh (scene->mMeshes[i], &rex_mesh);

        uint8_t *mat_ptr = rex_block_write_material (block_id, header, &rex_mat, &mat_data_sz[i]);
        rex_mesh.material_id = block_id;
        block_id++;
        uint8_t *mesh_ptr = rex_block_write_mesh (block_id, header, &rex_mesh, &mesh_data_sz[i]);
        block_id++;
        rex_mesh_free (&rex_mesh);
        mesh_data[i] = mesh_ptr;
        mat_data[i] = mat_ptr;
    }

    // write header blob
    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    // write data to file
    FILE *fp = fopen (settings.output, "wb");
    fwrite (header_ptr, header_sz, 1, fp);

    // write all mesh and material data
    for (i = 0; i < scene->mNumMeshes; i++)
    {
        fwrite (mat_data[i], mat_data_sz[i], 1, fp);
        fwrite (mesh_data[i], mesh_data_sz[i], 1, fp);
    }
    fclose (fp);
    return 0;
}
