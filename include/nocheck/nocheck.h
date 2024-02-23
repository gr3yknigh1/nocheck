#ifndef NOCHECK_NOCHECK_H_
#define NOCHECK_NOCHECK_H_

#include <stdio.h>
#include <stdlib.h>

typedef void (*N_proc_t)(void);

typedef enum {
    N_NONE,
    N_ERROR,
    N_PASS,
} N_test_case_status_t;

typedef struct {
    const char *expr;
    const char *file_path;
    unsigned long line_number;
} N_assert_expr_info_t;

typedef struct {
    N_proc_t init;
    N_proc_t fini;

    const char *_name;
    N_proc_t _run;
    N_test_case_status_t _status;
    N_assert_expr_info_t _last_expr_info;
} N_test_case_t;

typedef struct {
    N_proc_t init;
    N_proc_t fini;

    const char *_name;
    N_test_case_t *_cur_case;
} N_test_suite_t;

static N_test_suite_t N_TEST_SUITE;

#define _SUITE N_TEST_SUITE
#define _CUR_CASE N_TEST_SUITE._cur_case
#define _CUR_EXPR_INFO N_TEST_SUITE._cur_case->_last_expr_info
#define _CUR_STATUS N_TEST_SUITE._cur_case->_status

#define _CALL_PROC_IF_NOT_NULL(PROC)                                           \
    if (PROC != NULL) {                                                        \
        PROC();                                                                \
    }

#define N_TEST_CASE_MAX_COUNT 128
static N_test_case_t *N_TEST_CASES[N_TEST_CASE_MAX_COUNT];
static unsigned long N_TEST_CASE_COUNT = 0;

#define N_PRE_MAIN_PROC(NAME)                                                  \
    void NAME(void) __attribute__((constructor));                              \
    void NAME(void)

#define _TEST_RUN_NAME(__TEST_NAME) _##__TEST_NAME##_run
#define _TEST_STRUCT_NAME(__TEST_NAME) _##__TEST_NAME

#define _TEST_STRUCT(__TEST_NAME)                                              \
    static N_test_case_t _TEST_STRUCT_NAME(__TEST_NAME) =
#define _TEST_STRUCT_INIT(__TEST_NAME)                                         \
    N_PRE_MAIN_PROC(__TEST_NAME) {                                             \
        N_TEST_CASES[N_TEST_CASE_COUNT] = &_TEST_STRUCT_NAME(__TEST_NAME);     \
        N_TEST_CASE_COUNT++;                                                   \
    }

#define TEST(TEST_NAME)                                                        \
    void _TEST_RUN_NAME(TEST_NAME)(void);                                      \
    _TEST_STRUCT(TEST_NAME){                                                   \
        .init = NULL,                                                          \
        .fini = NULL,                                                          \
        ._name = #TEST_NAME,                                                   \
        ._run = _TEST_RUN_NAME(TEST_NAME),                                     \
        ._status = N_NONE,                                                     \
    };                                                                         \
    _TEST_STRUCT_INIT(TEST_NAME)                                               \
    void _TEST_RUN_NAME(TEST_NAME)(void)

#define TEST_PACK(PACK_NAME)                                                   \
    int main(void) {                                                           \
        int return_code = EXIT_SUCCESS;                                        \
        for (unsigned long i = 0; i < N_TEST_CASE_COUNT; ++i) {                \
            _CUR_CASE = N_TEST_CASES[i];                                       \
            _CALL_PROC_IF_NOT_NULL(_CUR_CASE->init);                           \
            _CUR_CASE->_run();                                                 \
            if (_CUR_CASE->_status == N_ERROR) {                               \
                printf("[%s::%s]: FAILED: '%s' in %s:%lu\n", #PACK_NAME,       \
                       _CUR_CASE->_name, _CUR_EXPR_INFO.expr,                  \
                       _CUR_EXPR_INFO.file_path, _CUR_EXPR_INFO.line_number);  \
                return_code = EXIT_FAILURE;                                    \
            }                                                                  \
            if (_CUR_STATUS == N_PASS) {                                       \
                printf("[%s::%s]: PASS\n", #PACK_NAME, _CUR_CASE->_name);      \
            }                                                                  \
            if (_CUR_STATUS == N_NONE) {                                       \
                printf("[%s::%s]: NONE\n", #PACK_NAME, _CUR_CASE->_name);      \
            }                                                                  \
            _CALL_PROC_IF_NOT_NULL(_CUR_CASE->fini);                           \
        }                                                                      \
        return return_code;                                                    \
    }                                                                          \
    static N_test_suite_t _SUITE = {._name = #PACK_NAME}

#define _SET_EXPR_INFO(__OUT_PTR, __EXPR)                                      \
    *(__OUT_PTR) = (N_assert_expr_info_t) {                                    \
        .expr = #__EXPR, .file_path = __FILE__, .line_number = __LINE__,       \
    }

#define _VALIDATE_EXPR(__OUT_STATUS, __EXPR)                                   \
    if ((__EXPR) != true) {                                                    \
        *(__OUT_STATUS) = N_ERROR;                                             \
    } else {                                                                   \
        *(__OUT_STATUS) = N_PASS;                                              \
    }

#define ASSERT(__EXPR)                                                         \
    do {                                                                       \
        _SET_EXPR_INFO(&_CUR_EXPR_INFO, __EXPR);                               \
        _VALIDATE_EXPR(&_CUR_STATUS, __EXPR);                                  \
    } while (0)

#endif // NOCHECK_NOCHECK_H_
