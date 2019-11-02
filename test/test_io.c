#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "unity.h"
#include "io.h"
#include "_struts.h"

#define TRY_FN_CATCH_EXCEPTION(fn_call)           \
e_actual = EXCEPTION_NO_EXCEPTION;                \
Try                                               \
{                                                 \
    fn_call;                                      \
    TEST_FAIL();                                  \
}                                                 \
Catch(e_actual)                                   \
    TEST_ASSERT_EQUAL_HEX_MESSAGE(e_expected, e_actual, "Encountered an unsuspected error condition!");

#define BACKSTORE_FILE_PATH "/tmp/test.io.bin"

int iofd;

blfs_backstore_t * fake_backstore;

void setUp(void)
{
    char buf[100] = { 0x00 };
    snprintf(buf, sizeof buf, "level%s_blfs_%s", STRINGIZE(BLFS_DEBUG_LEVEL), "test");

    if(dzlog_init(BLFS_CONFIG_ZLOG, buf))
        exit(EXCEPTION_ZLOG_INIT_FAILURE);

    iofd = open(BACKSTORE_FILE_PATH, O_CREAT | O_RDWR | O_TRUNC, 0777);

    fake_backstore = malloc(sizeof *fake_backstore);
    fake_backstore->io_fd = iofd;
    fake_backstore->body_real_offset = 128;
}

void tearDown(void)
{
    zlog_fini();
    close(fake_backstore->io_fd);
    unlink(BACKSTORE_FILE_PATH);
    free(fake_backstore);
}

void test_blfs_backstore_read_and_write_works_as_expected(void)
{
    int random_offset = 255;
    uint8_t buffer_actual1[1024] = { 0x00 };
    uint8_t buffer_actual2[64] = { 0x00 };
    uint8_t buffer_actual3[64] = { 0x00 };

    uint8_t * buffer_actual3_ptr = buffer_actual3;
    uint8_t * buffer_expected_zeroes = calloc(sizeof buffer_actual1, sizeof(uint8_t));

    blfs_backstore_write(fake_backstore, buffer_expected_zeroes, sizeof buffer_actual1, 0);
    blfs_backstore_read(fake_backstore, buffer_actual1, sizeof buffer_actual1, 0);

    TEST_ASSERT_EQUAL_MEMORY(buffer_expected_zeroes, buffer_actual1, sizeof buffer_actual1);

    uint8_t buffer_expected_random[sizeof buffer_actual2] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x01, 0x02, 0x03, 0x04
    };

    blfs_backstore_write(fake_backstore, buffer_expected_random, sizeof buffer_actual2, random_offset);
    blfs_backstore_read(fake_backstore, buffer_actual2, sizeof buffer_actual2, random_offset);

    TEST_ASSERT_EQUAL_MEMORY(buffer_expected_random, buffer_actual2, sizeof buffer_actual2);

    blfs_backstore_read(fake_backstore, buffer_actual3_ptr, sizeof(buffer_actual2) / 2, random_offset);
    blfs_backstore_read(fake_backstore,
                        buffer_actual3_ptr + sizeof(buffer_actual2) / 2,
                        sizeof(buffer_actual2) / 2,
                        random_offset + sizeof(buffer_actual2) / 2);

    TEST_ASSERT_EQUAL_MEMORY(buffer_expected_random, buffer_actual3, sizeof buffer_actual3);
}

void test_blfs_backstore_read_body_and_write_body_works_as_expected(void)
{
    uint8_t buffer_actual1[45] = { 0x00 };
    uint8_t buffer_actual2[256] = { 0x00 };

    uint8_t buffer_expected_random[sizeof buffer_actual2] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06
    };

    uint8_t buffer_expected_welldefined[sizeof buffer_actual2] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06
    };

    uint8_t buffer_expected_head1[sizeof buffer_actual1] = {
        0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x05, 0x06, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04
    };

    uint8_t buffer_expected_head2[16] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    blfs_backstore_write(fake_backstore, buffer_expected_random, sizeof buffer_expected_random, 0);
    blfs_backstore_read_body(fake_backstore, buffer_actual1, sizeof buffer_actual1, 19);

    TEST_ASSERT_EQUAL_MEMORY(buffer_expected_head1, buffer_actual1, sizeof buffer_actual1);

    blfs_backstore_write_body(fake_backstore, buffer_expected_head2, sizeof buffer_expected_head2, 0);
    blfs_backstore_read(fake_backstore, buffer_actual2, sizeof buffer_actual2, 0);

    TEST_ASSERT_EQUAL_MEMORY(buffer_expected_welldefined, buffer_actual2, sizeof buffer_actual2);
}

void test_blfs_backstore_create_work_as_expected(void)
{
    unlink(BACKSTORE_FILE_PATH);

    blfs_backstore_t * backstore = blfs_backstore_create(BACKSTORE_FILE_PATH, 4096);

    TEST_ASSERT_EQUAL_STRING(BACKSTORE_FILE_PATH, backstore->file_path);
    TEST_ASSERT_EQUAL_STRING("test.io.bin", backstore->file_name);
    TEST_ASSERT_EQUAL_UINT(0, backstore->kcs_real_offset);
    TEST_ASSERT_EQUAL_UINT(0, backstore->tj_real_offset);
    TEST_ASSERT_EQUAL_UINT(0, backstore->md_real_offset);
    TEST_ASSERT_EQUAL_UINT(0, backstore->body_real_offset);
    TEST_ASSERT_EQUAL_UINT(0, backstore->writeable_size_actual);
    TEST_ASSERT_EQUAL_UINT(0, backstore->nugget_size_bytes);
    TEST_ASSERT_EQUAL_UINT(0, backstore->flake_size_bytes);
    TEST_ASSERT_EQUAL_UINT(1, backstore->md_bytes_per_nugget);
    TEST_ASSERT_EQUAL_UINT(0, backstore->num_nuggets);
    TEST_ASSERT_EQUAL_UINT(0, backstore->flakes_per_nugget);
    TEST_ASSERT_EQUAL_UINT(4096, backstore->file_size_actual);
}

void test_blfs_backstore_create_throws_exception_if_backstore_file_already_exists(void)
{
    CEXCEPTION_T e_expected = EXCEPTION_FILE_ALREADY_EXISTS;
    volatile CEXCEPTION_T e_actual = EXCEPTION_NO_EXCEPTION;

    TRY_FN_CATCH_EXCEPTION((void) blfs_backstore_create(BACKSTORE_FILE_PATH, 4096));
}

void test_blfs_backstore_open_work_as_expected(void)
{
    blfs_backstore_write(fake_backstore, buffer_init_backstore_state, sizeof buffer_init_backstore_state, 0);

    blfs_backstore_t * backstore = blfs_backstore_open(BACKSTORE_FILE_PATH);
    blfs_backstore_t backstore2;

    memcpy(&backstore2, backstore, sizeof backstore2);

    TEST_ASSERT_EQUAL_STRING(BACKSTORE_FILE_PATH, backstore->file_path);
    TEST_ASSERT_EQUAL_STRING("test.io.bin", backstore->file_name);
    TEST_ASSERT_EQUAL_UINT(105, backstore->kcs_real_offset);
    TEST_ASSERT_EQUAL_UINT(129, backstore->tj_real_offset);
    TEST_ASSERT_EQUAL_UINT(132, backstore->md_real_offset);
    TEST_ASSERT_EQUAL_UINT(0, backstore->body_real_offset);

    blfs_backstore_setup_actual_finish(backstore);

    TEST_ASSERT_EQUAL_UINT(132, backstore->body_real_offset);
    TEST_ASSERT_EQUAL_UINT(48, backstore->writeable_size_actual);
    TEST_ASSERT_EQUAL_UINT(16, backstore->nugget_size_bytes);
    TEST_ASSERT_EQUAL_UINT(8, backstore->flake_size_bytes);
    TEST_ASSERT_EQUAL_UINT(3, backstore->num_nuggets);
    TEST_ASSERT_EQUAL_UINT(2, backstore->flakes_per_nugget);
    TEST_ASSERT_EQUAL_UINT(204, backstore->file_size_actual);

    backstore2.md_bytes_per_nugget = 8;

    blfs_backstore_setup_actual_finish(&backstore2);

    TEST_ASSERT_EQUAL_UINT(156, backstore2.body_real_offset);
    TEST_ASSERT_EQUAL_UINT(48, backstore2.writeable_size_actual);
    TEST_ASSERT_EQUAL_UINT(16, backstore2.nugget_size_bytes);
    TEST_ASSERT_EQUAL_UINT(8, backstore2.flake_size_bytes);
    TEST_ASSERT_EQUAL_UINT(3, backstore2.num_nuggets);
    TEST_ASSERT_EQUAL_UINT(2, backstore2.flakes_per_nugget);
    TEST_ASSERT_EQUAL_UINT(204, backstore2.file_size_actual);
}

void test_blfs_backstore_close_work_as_expected(void)
{
    blfs_backstore_write(fake_backstore, buffer_init_backstore_state, sizeof buffer_init_backstore_state, 0);
    blfs_backstore_t * backstore = blfs_backstore_open(BACKSTORE_FILE_PATH);
    blfs_backstore_close(backstore); // No errors? All good!
}
