#include <check.h>
#include <stdio.h>

#include "global.h"
#include "rex-block.h"
#include "rex-block-mesh.h"
#include "rex-header.h"
#include "status.h"
#include "util.h"

#define REX_TEMPLATE "template.rex"

char tmp[256];

void check_template_header(struct rex_header *header)
{
    ck_assert (header->version == REX_FILE_VERSION);
    ck_assert (header->crc == 0);
    ck_assert (header->nr_datablocks == 4);
    ck_assert (header->start_addr == 86);
    ck_assert (header->sz_all_datablocks == 430);
}

#if 0
START_TEST (test_rex_header_read)
{
    struct rex_header *header = rex_header_create();
    header->crc = 0;
    header->nr_datablocks = 4;
    header->start_addr = 86;
    header->sz_all_datablocks = 430;
    check_template_header(&header);
}
END_TEST

START_TEST (test_rex_header_write)
{
    char *name = "test.rex";

    // read
    sprintf (tmp, "%s/%s", TEST_DATA_PATH, REX_TEMPLATE);
    struct rex_header header;
    FILE *fp = read_header(path, &header);
    ck_assert (fp != NULL);

    // write
    FILE *fpout = fopen (name, "wb");
    ck_assert (fpout != NULL);
    ck_assert (rex_header_write (fpout, &header) == REX_OK);
    ck_assert (fclose(fp) == 0);
    ck_assert (fclose(fpout) == 0);

    // read
    fp = read_header(name, &header);
    check_template_header(&header);
    ck_assert (fclose(fp) == 0);

    remove("test.rex");
}
END_TEST
#endif

START_TEST (test_rex_reader)
{
    sprintf (tmp, "%s/%s", TEST_DATA_PATH, REX_TEMPLATE);
    long sz;
    uint8_t *buf = read_file_binary(tmp, &sz);
    ck_assert(buf != NULL);
    ck_assert(sz == 516);

    struct rex_header header;
    ck_assert(rex_header_read(buf, &header) == (buf+86));
    check_template_header(&header);
    uint8_t *ptr = buf+86;

    ck_assert(header.nr_datablocks == 4);

    struct rex_block block;

    // Mesh
    ptr = rex_block_read(ptr, &block);
    ck_assert(block.id == 0);
    ck_assert(block.type == 3);
    struct rex_mesh *mesh = block.data;
    ck_assert(mesh->nr_vertices == 3);
    ck_assert(mesh->nr_triangles == 1);
    ck_assert(mesh->material_id == 1);
    ck_assert(strcmp(mesh->name, "mesh") == 0);

    // MaterialStandard
    ptr = rex_block_read(ptr, &block);
    ck_assert(block.id == 1);
    ck_assert(block.type == 5);

    // PeopleSimulation
    ptr = rex_block_read(ptr, &block);
    ck_assert(block.id == 2);
    ck_assert(block.type == 6);

    // UnityPackage
    ptr = rex_block_read(ptr, &block);
    ck_assert(block.id == 3);
    ck_assert(block.type == 7);

    FREE(buf);
}
END_TEST

Suite *test_suite()
{
    Suite *s;
    TCase *tc_io;

    s = suite_create ("main");

    /* io test case */
    tc_io = tcase_create ("io");

    /* tcase_add_test (tc_io, test_rex_header_read); */
    /* tcase_add_test (tc_io, test_rex_header_write); */
    tcase_add_test (tc_io, test_rex_reader);

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
