//*****************************************************************************
// talloc
//
// File:   talloc_test.c
// Author: Martin Dorazil
// Date:   19/05/2017
//
// Copyright 2017 Martin Dorazil
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//*****************************************************************************

#include <check.h>
#include "talloc/talloc.h"

// maximum size for 512 will be 4104 bytes (we test also large allocations)
#define TEST_SIZES_COUNT 512 
#define TEST_COUNT 10000 
#define TEST_BUFFER_SIZE 256 

#define test_size_for_id(id) (sizeof(intptr_t) + sizeof(intptr_t) * ((id) % TEST_SIZES_COUNT))
#define buffer_id(id) ((id) % TEST_BUFFER_SIZE)

static void *test_data_ptrs[TEST_BUFFER_SIZE] = {0};

START_TEST(test_allocation)
{
    int buf_id = 0;
    for (int i = 0; i < TEST_COUNT; i++) {
        buf_id = buffer_id(i);
        if (test_data_ptrs[buf_id] != NULL)
        {
            ck_assert_uint_eq(*(intptr_t *)test_data_ptrs[buf_id], (intptr_t)test_data_ptrs[buf_id]);
            tfree(test_data_ptrs[buf_id]);
        }

        test_data_ptrs[buf_id] = tmalloc(test_size_for_id(buf_id));
        *(intptr_t *)test_data_ptrs[buf_id] = (intptr_t)test_data_ptrs[buf_id];
    }

    for (int i = 0; i < TEST_BUFFER_SIZE; i++) {
        tfree(test_data_ptrs[i]);
        test_data_ptrs[i] = NULL;
    }
}
END_TEST

static Suite *
talloc_suite(void)
{
    // suite
    Suite *suite = suite_create("talloc");

    // test cases
    TCase *tcase = tcase_create("test_allocation");
    tcase_add_test(tcase, test_allocation);

    suite_add_tcase(suite, tcase);

    return suite;
}

int
main(int argc, char *argv[])
{
    int number_failed;
    Suite *suite = talloc_suite();
    SRunner *runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return number_failed;
}
