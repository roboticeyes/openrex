#include <check.h>
#include <stdio.h>

#include "global.h"
#include "rex-header.h"
#include "status.h"

#define REX_TEMPLATE "template.rex"

START_TEST (test_rex_header_read)
{
    struct rex_header header;
    char path[256];
    sprintf (path, "%s/%s", TEST_DATA_PATH, REX_TEMPLATE);
    FILE *fp = fopen (path, "rb");

    ck_assert (fp);
    ck_assert (rex_header_read (fp, &header) == REX_OK);

    ck_assert (header.version == REX_FILE_VERSION);
    ck_assert (header.crc == 0);
    ck_assert (header.nr_datablocks == 2);
    ck_assert (header.start_addr == 0);
    ck_assert (header.sz_all_datablocks == 0);

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
