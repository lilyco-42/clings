/* 60_micro-test-framework — Micro C Unit Test Framework (solution) */

#include <math.h>
#include <stdio.h>
#include <string.h>

/* ─── Global counters ─── */
static int test_count = 0;
static int fail_count = 0;

/* ─── Assertion macros ─── */
#define ASSERT_EQ(a, b)                                          \
    do {                                                         \
        if ((a) != (b)) {                                        \
            printf(                                              \
                "  FAIL %s:%d: ASSERT_EQ(%s, %s) "               \
                "— expected %d, got %d\n",                       \
                __FILE__, __LINE__, #a, #b, (int)(b), (int)(a)); \
            fail_count++;                                        \
            return;                                              \
        }                                                        \
    } while (0)

#define ASSERT_STREQ(a, b)                                                                             \
    do {                                                                                               \
        const char *_aa = (a);                                                                         \
        const char *_bb = (b);                                                                         \
        if ((_aa == NULL) != (_bb == NULL) || (_aa != NULL && _bb != NULL && strcmp(_aa, _bb) != 0)) { \
            printf(                                                                                    \
                "  FAIL %s:%d: ASSERT_STREQ(%s, %s) "                                                  \
                "— expected \"%s\", got \"%s\"\n",                                                     \
                __FILE__, __LINE__, #a, #b, _bb ? _bb : "(null)", _aa ? _aa : "(null)");               \
            fail_count++;                                                                              \
            return;                                                                                    \
        }                                                                                              \
    } while (0)

/* ─── TEST macro ─── */
#define TEST(name) static void test_##name(void)

/* ─── Test runner ─── */
typedef struct {
    void (*func)(void);
    const char *name;
} TestEntry;

static TestEntry tests[64];
static int test_index = 0;

#define RUN_TESTS()                                                                                  \
    do {                                                                                             \
        test_count = test_index;                                                                     \
        printf("Running %d test(s)...\n\n", test_index);                                             \
        for (int _i = 0; _i < test_index; _i++) {                                                    \
            printf("[%s] ", tests[_i].name);                                                         \
            int _before = fail_count;                                                                \
            tests[_i].func();                                                                        \
            if (fail_count == _before) {                                                             \
                printf("PASS\n");                                                                    \
            }                                                                                        \
        }                                                                                            \
        printf("\n─── Summary ───\n");                                                               \
        printf("%d tests, %d passed, %d failed\n", test_count, test_count - fail_count, fail_count); \
    } while (0)

/* ─── Register test ─── */
static void register_test(void (*func)(void), const char *name) {
    tests[test_index].func = func;
    tests[test_index].name = name;
    test_index++;
}

/* ─── Test cases ─── */

TEST(add) {
    ASSERT_EQ(1 + 1, 2);
    ASSERT_EQ(3 * 7, 21);
    ASSERT_EQ(100 - 50, 50);
}

TEST(str) {
    ASSERT_STREQ("hello", "hello");
    ASSERT_STREQ("C", "C");
    char buf[] = "clings";
    ASSERT_STREQ(buf, "clings");
}

TEST(fail) { ASSERT_EQ(2 + 2, 5); /* deliberately wrong */ }

/* test_null: 验证 ASSERT_STREQ 对 NULL 指针的处理 */
TEST(null) {
    /* NULL vs NULL — 两个都是 NULL 应该通过 (都是空指针) */
    ASSERT_STREQ(NULL, NULL);

    /* NULL vs 有效字符串 — 应该失败 */
    ASSERT_STREQ(NULL, "hello");
}

/* test_float: 验证 ASSERT_EQ 对浮点/双精度取整的容差行为
 * ASSERT_EQ 比较的是 int，所以 double 会被截断为 int 比较。
 * 这里用 (int) 显式转换展示双精度比较的局限。 */
TEST(float) {
    /* 3.14 → (int)3, 3 → 3, 相等通过 */
    ASSERT_EQ((int)3.14, 3);

    /* 2.718 → (int)2, 3 → 3, 不相等 → 故意失败 */
    ASSERT_EQ((int)2.718, 3);
}

/* ─── Main ─── */
int main(void) {
    register_test(test_add, "test_add");
    register_test(test_str, "test_str");
    register_test(test_fail, "test_fail");
    register_test(test_null, "test_null");
    register_test(test_float, "test_float");

    RUN_TESTS();
    return 0;
}
