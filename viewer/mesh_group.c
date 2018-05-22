#include "mesh_group.h"
#include "list.h"

void mesh_group_init (struct mesh_group *g)
{
    if (!g) return;
    bbox_init (&g->bb);
    mat4x4_identity(g->transform);

    g->meshes = list_create();
}

void mesh_group_destroy(struct mesh_group *g)
{
    if (g->meshes && g->meshes->head)
    {
        struct node *cur = g->meshes->head;
        while (cur)
        {
            struct mesh *m = cur->data;
            rex_mesh_free (m->data);
            mesh_free (m);
            cur = cur->next;
        }
    }
    list_destroy (g->meshes);
}

void mesh_group_add_mesh (struct mesh_group *g, struct mesh *m)
{
    if (!g) return;
    list_insert (g->meshes, m);
    mesh_group_calc_bbox(g);
}

void mesh_group_remove_mesh (struct mesh_group *g, struct mesh *m)
{
    if (!g || !g->meshes || !g->meshes->head) return;
    struct node *cur = g->meshes->head;
    while (cur)
    {
        if (cur->data == m)
        {
            list_delete_node (g->meshes, cur);
            break;
        }
        cur = cur->next;
    }

    // update bounding box
    mesh_group_calc_bbox(g);
}

void mesh_group_calc_bbox (struct mesh_group *g)
{
    if (!g || !g->meshes || !g->meshes->head) return;
    bbox_init(&g->bb);

    struct node *cur = g->meshes->head;
    while (cur)
    {
        struct mesh *m = cur->data;
        bbox_union(&g->bb, &g->bb, &m->bb);
        cur = cur->next;
    }
    printf("Mesh group bounding box:\n");
    vec3_dump("  min", g->bb.min);
    vec3_dump("  max", g->bb.max);
}

void mesh_group_center (struct mesh_group *g)
{
    if (!g) return;

    float tx = -(g->bb.min[0] + (g->bb.max[0] - g->bb.min[0]) /2.0);
    float ty = -(g->bb.min[1] + (g->bb.max[1] - g->bb.min[1]) /2.0);
    float tz = -(g->bb.min[2] + (g->bb.max[2] - g->bb.min[2]) /2.0);

    mat4x4_translate(g->transform, tx, ty, tz);
    mat4x4_dump(g->transform);
}

void mesh_group_render (struct mesh_group *g, struct shader *s, struct camera *cam, mat4x4 projection)
{
    if (!g || !g->meshes || !g->meshes->head) return;
    struct node *cur = g->meshes->head;
    while (cur)
    {
        mesh_render (cur->data, s, g->transform, cam, projection);
        cur = cur->next;
    }
}
