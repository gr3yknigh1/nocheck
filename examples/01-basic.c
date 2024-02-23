#include <nocheck/nocheck.h>

TEST_PACK(basic); // One per translation unit.

TEST(test0) {
    ASSERT(1 == 1);
    ASSERT(true);
}

TEST(test1) {
    ASSERT(false); // will fail.
}

/*
 * Output:
 *
 * $ gcc ./examples/01-basic.c -std=c2x -I include -o 01-basic
 * $ ./01-basic
 * [basic::test0]: PASS
 * [basic::test1]: FAILED: 'false' in ./examples/01-basic.c:11
 *
 * $
 *
 * */
