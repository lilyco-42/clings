/* 60_micro-test-framework — Micro C Unit Test Framework
 *
 * Task: Implement a tiny unit test framework entirely with C macros.
 * Write ASSERT_EQ, ASSERT_STREQ, TEST, and RUN_TESTS macros, then
 * write 5 test cases to verify the framework works (2 deliberate failures).
 *
 * Knowledge points:
 *   - C preprocessor: # (stringify), ## (token paste), do-while(0) idiom
 *   - Macro design: multi-statement macros, argument evaluation
 *   - Unit testing concepts: assertions, test registration, test runner
 *   - Function pointers and struct arrays for test registry
 *   - NULL handling in string assertions
 *   - Floating-point / double comparison limitations with integer assertions
 *
 * Verification (expected output matches expected_output.txt):
 *   ./test_framework
 *   Running 5 test(s)...
 *   [test_add] PASS
 *   [test_str] PASS
 *   [test_fail]   FAIL test_framework.c:XX: ASSERT_EQ(2 + 2, 5) ...
 *   [test_null]   FAIL test_framework.c:XX: ASSERT_STREQ(NULL, "hello") ...
 *   [test_float]   FAIL test_framework.c:XX: ASSERT_EQ((int)2.718, 3) ...
 *   ─── Summary ───
 *   5 tests, 2 passed, 3 failed
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

/* ─── Global counters ─── */
static int test_count = 0;
static int fail_count = 0;

/* ─── TODO 1: ASSERT_EQ macro ─── */
#error TODO 1: Define ASSERT_EQ(a, b) macro.
/*
 * ASSERT_EQ(a, b) should:
 *   1. Compare (a) != (b) — wrap args in parentheses to avoid precedence bugs.
 *   2. If they differ, print a FAIL message with __FILE__, __LINE__,
 *      #a (stringified a), #b, (int)(b) as expected, (int)(a) as got.
 *      Increment fail_count and return from the test function.
 *   3. Use the do { ... } while(0) idiom so the macro works as a
 *      single statement everywhere (e.g. if/else without braces).
 *
 *   Format string example:
 *     "  FAIL %s:%d: ASSERT_EQ(%s, %s) — expected %d, got %d\n"
 *     args: __FILE__, __LINE__, #a, #b, (int)(b), (int)(a)
 */

/* ─── TODO 2: ASSERT_STREQ macro ─── */
#error TODO 2: Define ASSERT_STREQ(a, b) macro.
/*
 * ASSERT_STREQ(a, b) should:
 *   1. Store (a) and (b) in const char* locals to avoid double-evaluation.
 *   2. Check for failure: (one NULL AND the other not NULL) OR
 *      (both non-NULL AND strcmp() != 0).
 *      If both are NULL, they are considered equal (no failure).
 *   3. On failure: print FAIL with __FILE__, __LINE__, #a, #b,
 *      expected string (or "(null)"), got string (or "(null)").
 *      Increment fail_count and return.
 *   4. Use do { ... } while(0).
 *
 *   Format string example:
 *     "  FAIL %s:%d: ASSERT_STREQ(%s, %s) — expected \"%s\", got \"%s\"\n"
 */

/* ─── TODO 3: TEST macro ─── */
#error TODO 3: Define TEST(name) macro.
/*
 * TEST(name) should expand to:
 *   static void test_##name(void)
 *
 * This uses ## (token pasting) to create a function named test_add
 * when the user writes TEST(add) { ... }.
 *
 * The function must be static void — no arguments, no return value.
 */

/* ─── Test runner data structures ─── */
typedef struct {
    void (*func)(void);
    const char *name;
} TestEntry;

static TestEntry tests[64];
static int test_index = 0;

/* ─── TODO 4: RUN_TESTS macro ─── */
#error TODO 4: Define RUN_TESTS() macro.
/*
 * RUN_TESTS() should:
 *   1. Set test_count = test_index (total number of TEST blocks registered).
 *   2. Print "Running %d test(s)...\n\n" with test_index.
 *   3. Loop over tests[] from 0 to test_index-1:
 *      - Print "[%s] " with the test name.
 *      - Record fail_count before calling the test function.
 *      - Call tests[_i].func().
 *      - If fail_count didn't change, print "PASS\n".
 *        (If the test failed, the assertion already printed the FAIL line.)
 *   4. Print "\n─── Summary ───\n" and
 *      "%d tests, %d passed, %d failed\n" with
 *      test_count, test_count - fail_count, fail_count.
 *   5. Use do { ... } while(0).
 */

/* ─── Register test ─── */
static void register_test(void (*func)(void), const char *name) {
    tests[test_index].func = func;
    tests[test_index].name = name;
    test_index++;
}

/* ─── TODO 5: Write 5 test cases ─── */
#error TODO 5: Write test_add, test_str, test_fail, test_null, test_float using the TEST macro.
/*
 * Use TEST(name) { ... } to define each test case.
 *
 * test_add: use ASSERT_EQ to check:
 *   1 + 1 == 2
 *   3 * 7 == 21
 *   100 - 50 == 50
 *
 * test_str: use ASSERT_STREQ to check:
 *   "hello" equals "hello"
 *   "C" equals "C"
 *   a char array "clings" equals "clings"
 *
 * test_fail: deliberately write a failing assertion:
 *   ASSERT_EQ(2 + 2, 5);  — this should fail and report the mismatch.
 *
 * test_null: test ASSERT_STREQ NULL pointer handling:
 *   ASSERT_STREQ(NULL, NULL);     — both NULL, should PASS
 *   ASSERT_STREQ(NULL, "hello");  — NULL vs string, should FAIL
 *
 * test_float: test ASSERT_EQ with double-to-int truncation:
 *   ASSERT_EQ((int)3.14, 3);      — 3 == 3, should PASS
 *   ASSERT_EQ((int)2.718, 3);     — 2 != 3, should FAIL
 *   (Note: ASSERT_EQ compares int values, so double must be cast to int.
 *    This demonstrates the precision limitation — use a tolerance-based
 *    approach for true floating-point comparison.)
 */

/* ─── TODO 6: main() — register and run tests ─── */
#error TODO 6: Complete main().
/*
 * In main():
 *   1. Call register_test() five times to register:
 *      test_add as "test_add"
 *      test_str as "test_str"
 *      test_fail as "test_fail"
 *      test_null as "test_null"
 *      test_float as "test_float"
 *   2. Call RUN_TESTS().
 *   3. Return 0.
 *
 *   The register_test() function and TestEntry/tests[]/test_index
 *   are already provided above. Just call register_test 5 times
 *   then RUN_TESTS().
 */

int main(void) {
#error TODO 6: Register the five test functions and call RUN_TESTS().
    return 0;
}
