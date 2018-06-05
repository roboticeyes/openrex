#include <check.h>
#include <stdio.h>

#include "rex.h"

#define REX_TEMPLATE "template.rex"

char tmp[256];

void check_template_header (struct rex_header *header)
{
    ck_assert (header->version == REX_FILE_VERSION);
    ck_assert (header->crc == 0);
    ck_assert (header->nr_datablocks == 4);
    ck_assert (header->start_addr == 86);
    ck_assert (header->sz_all_datablocks == 430);
}

void generate_mesh (struct rex_mesh *mesh)
{
    ck_assert (mesh != NULL);

    rex_mesh_init (mesh);

    mesh->nr_vertices = 3;
    mesh->nr_triangles = 1;

    mesh->positions = malloc (12 * 3);
    vec3 v1 = { 0.0, 0.0, 0.0 };
    vec3 v2 = { 1.0, 0.0, 0.0 };
    vec3 v3 = { 0.5, 1.0, 0.0 };
    memcpy (mesh->positions, v1, 12);
    memcpy (&mesh->positions[3], v2, 12);
    memcpy (&mesh->positions[6], v3, 12);
    mesh->triangles = malloc (12);
    mesh->triangles[0] = 0;
    mesh->triangles[1] = 1;
    mesh->triangles[2] = 2;
    mesh->material_id = 0;

    sprintf (mesh->name, "test");
}

void generate_material (struct rex_material_standard *mat)
{
    ck_assert (mat != NULL);

    mat->ka_red = 1.0f;
    mat->ka_green = 0.0f;
    mat->ka_blue = 0.0f;
    mat->ka_textureId = REX_NOT_SET;
    mat->kd_red = 1.0f;
    mat->kd_green = 0.0f;
    mat->kd_blue = 0.0f;
    mat->kd_textureId = REX_NOT_SET;
    mat->ks_red = 1.0f;
    mat->ks_green = 0.0f;
    mat->ks_blue = 0.0f;
    mat->ks_textureId = REX_NOT_SET;
    mat->ns = 0.0f;
    mat->alpha = 1.0f;
}

void generate_lineset (struct rex_lineset *ls)
{
    ck_assert (ls != NULL);
    ls->red = 0.7f;
    ls->green = 0.0f;
    ls->blue = 0.0f;
    ls->nr_vertices = 5;
    vec3 v1 = { 0.0, 0.0, 0.0 };
    vec3 v2 = { 1.0, 0.0, 0.0 };
    vec3 v3 = { 1.0, 1.0, 0.0 };
    vec3 v4 = { 0.0, 1.0, 0.0 };
    ls->vertices = malloc (12 * 5);
    memset (ls->vertices, 0, 12 * 5);

    memcpy (ls->vertices, v1, 12);
    memcpy (&ls->vertices[3], v2, 12);
    memcpy (&ls->vertices[6], v3, 12);
    memcpy (&ls->vertices[9], v4, 12);
    memcpy (&ls->vertices[12], v1, 12);
}

void generate_pointlist (struct rex_pointlist *p, int color)
{
    ck_assert (p != NULL);
    rex_pointlist_init (p);

    p->nr_vertices = 100;

    p->positions = malloc (12 * p->nr_vertices);
    int i = 0;
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            p->positions[i++] = (float) x;
            p->positions[i++] = (float) y;
            p->positions[i++] = 1.0f;
        }
    }

    if (color)
    {
        p->nr_colors = 100;
        p->colors = malloc (12 * p->nr_colors);
        for (int i = 0; i < p->nr_colors * 3; i += 3)
        {
            p->colors[i] = 0.8f;
            p->colors[i + 1] = 0.0f;
            p->colors[i + 2] = 0.0f;
        }
    }
    else
        p->nr_colors = 0;
}

START_TEST (test_general)
{
    ck_assert (strcmp (rex_name, "REX") == 0);
}
END_TEST

START_TEST (test_rex_writer_pointlist_nocolor)
{
    struct rex_header *header = rex_header_create();

    struct rex_pointlist p;
    generate_pointlist (&p, 0);
    long p_sz;
    uint8_t *p_ptr = rex_block_write_pointlist (0 /*id*/, header, &p, &p_sz);
    ck_assert (p_ptr != NULL);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    const char *filename = "test_pointlist_nocolor.rex";
    FILE *fp = fopen (filename, "wb");

    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (p_ptr, p_sz, 1, fp);
    fclose (fp);
}
END_TEST

START_TEST (test_rex_writer_pointlist_color)
{
    struct rex_header *header = rex_header_create();

    struct rex_pointlist p;
    generate_pointlist (&p, 1);
    long p_sz;
    uint8_t *p_ptr = rex_block_write_pointlist (0 /*id*/, header, &p, &p_sz);
    ck_assert (p_ptr != NULL);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    const char *filename = "test_pointlist_color.rex";
    FILE *fp = fopen (filename, "wb");

    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (p_ptr, p_sz, 1, fp);
    fclose (fp);
}
END_TEST

START_TEST (test_rex_writer_mesh)
{
    struct rex_header *header = rex_header_create();

    struct rex_material_standard mat;
    generate_material (&mat);
    long mat_sz;
    uint8_t *mat_ptr = rex_block_write_material (0 /*id*/, header, &mat, &mat_sz);
    ck_assert (mat_ptr != NULL);

    struct rex_mesh mesh;
    generate_mesh (&mesh);
    long mesh_sz;
    uint8_t *mesh_ptr = rex_block_write_mesh (1 /*id*/, header, &mesh, &mesh_sz);
    ck_assert (mesh_ptr != NULL);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);

    const char *filename = "test_mesh.rex";
    FILE *fp = fopen (filename, "wb");

    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (mesh_ptr, mesh_sz, 1, fp);
    fwrite (mat_ptr, mat_sz, 1, fp);
    fclose (fp);
}
END_TEST

START_TEST (test_rex_writer_lineset_and_text)
{
    struct rex_header *header = rex_header_create();

    struct rex_lineset ls;
    generate_lineset (&ls);
    long ls_sz;
    uint8_t *ls_ptr = rex_block_write_lineset (0 /*id*/, header, &ls, &ls_sz);
    ck_assert (ls_sz == 92);
    ck_assert (header->sz_all_datablocks == 92);
    ck_assert (ls_ptr != NULL);

    struct rex_text text =
    {
        .position = { 0.0, 1.0, 0.0 },
        .font_size = 24.0f,
        .data = "1 Meter"
    };
    long text_sz;
    uint8_t *text_ptr = rex_block_write_text (1 /*id*/, header, &text, &text_sz);
    ck_assert_msg (text_sz == 41, "actual %d", text_sz);
    ck_assert (header->sz_all_datablocks == 133);
    ck_assert (text_ptr != NULL);

    long header_sz;
    uint8_t *header_ptr = rex_header_write (header, &header_sz);
    ck_assert (header_sz == 86);

    const char *filename = "test_lineset_and_text.rex";
    FILE *fp = fopen (filename, "wb");

    fwrite (header_ptr, header_sz, 1, fp);
    fwrite (ls_ptr, ls_sz, 1, fp);
    fwrite (text_ptr, text_sz, 1, fp);
    fclose (fp);
}
END_TEST

START_TEST (test_rex_reader)
{
    sprintf (tmp, "%s/%s", TEST_DATA_PATH, REX_TEMPLATE);
    long sz;
    uint8_t *buf = read_file_binary (tmp, &sz);
    ck_assert (buf != NULL);
    ck_assert (sz == 516);

    struct rex_header header;
    ck_assert (rex_header_read (buf, &header) == (buf + 86));
    check_template_header (&header);
    uint8_t *ptr = buf + 86;

    ck_assert (header.nr_datablocks == 4);

    struct rex_block block;

    // Mesh
    ptr = rex_block_read (ptr, &block);
    ck_assert (block.id == 0);
    ck_assert (block.type == 3);
    struct rex_mesh *mesh = block.data;
    ck_assert (mesh->nr_vertices == 3);
    ck_assert (mesh->nr_triangles == 1);
    ck_assert (mesh->material_id == 1);
    ck_assert (strcmp (mesh->name, "mesh") == 0);

    // MaterialStandard
    ptr = rex_block_read (ptr, &block);
    ck_assert (block.id == 1);
    ck_assert (block.type == 5);
    struct rex_material_standard *mat = block.data;
    ck_assert (mat->ka_red == 1.0f);
    ck_assert (mat->ka_textureId  == REX_NOT_SET);

    // PeopleSimulation
    ptr = rex_block_read (ptr, &block);
    ck_assert (block.id == 2);
    ck_assert (block.type == 6);

    // UnityPackage
    ptr = rex_block_read (ptr, &block);
    ck_assert (block.id == 3);
    ck_assert (block.type == 7);

    FREE (buf);
}
END_TEST

Suite *test_suite()
{
    Suite *s;
    TCase *tc_general;
    TCase *tc_io;

    s = suite_create ("main");

    /* general test case */
    tc_general = tcase_create ("general");
    tcase_add_test (tc_general, test_general);

    /* io test case */
    tc_io = tcase_create ("io");
    tcase_add_test (tc_io, test_rex_reader);
    tcase_add_test (tc_io, test_rex_writer_mesh);
    tcase_add_test (tc_io, test_rex_writer_lineset_and_text);
    tcase_add_test (tc_io, test_rex_writer_pointlist_color);
    tcase_add_test (tc_io, test_rex_writer_pointlist_nocolor);

    suite_add_tcase (s, tc_general);
    suite_add_tcase (s, tc_io);
    return s;
}

int main()
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = test_suite();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (number_failed == 0) ? 0 : 1;
}
