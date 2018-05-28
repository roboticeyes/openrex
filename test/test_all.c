#include <check.h>
#include <stdio.h>

#include "global.h"
#include "rex-header.h"
#include "status.h"

#define REX_TEMPLATE "template.rex"

/**
 * opens the template file and returns the file pointer and the rex header
 */
FILE* read_header(const char* path, struct rex_header *header)
{
    FILE *fp = fopen (path, "rb");
    ck_assert (fp);
    ck_assert(rex_header_read (fp, header)== REX_OK);
    return fp;
}

void check_template_header(struct rex_header *header)
{
    ck_assert (header->version == REX_FILE_VERSION);
    ck_assert (header->crc == 0);
    ck_assert (header->nr_datablocks == 4);
    ck_assert (header->start_addr == 86);
    ck_assert (header->sz_all_datablocks == 430);
}

START_TEST (test_rex_header_read)
{
    char path[256];
    sprintf (path, "%s/%s", TEST_DATA_PATH, REX_TEMPLATE);
    struct rex_header header;
    FILE *fp = read_header(path, &header);
    check_template_header(&header);
    ck_assert (fclose(fp) == 0);
}
END_TEST

START_TEST (test_rex_header_write)
{
    char *name = "test.rex";

    // read
    char path[256];
    sprintf (path, "%s/%s", TEST_DATA_PATH, REX_TEMPLATE);
    struct rex_header header;
    FILE *fp = read_header(path, &header);
    ck_assert (fp);

    // write
    FILE *fpout = fopen (name, "wb");
    ck_assert (fpout);
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

Suite *test_suite()
{
    Suite *s;
    TCase *tc_io;

    s = suite_create ("main");

    /* io test case */
    tc_io = tcase_create ("io");

    tcase_add_test (tc_io, test_rex_header_read);
    tcase_add_test (tc_io, test_rex_header_write);

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
