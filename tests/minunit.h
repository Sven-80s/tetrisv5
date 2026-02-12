/**
 * @file minunit.h
 * @brief Minimal unit testing framework for C (single header)
 * 
 * Based on the original minunit by Jera Design,
 * modified for this project.
 */

#ifndef MINUNIT_H
#define MINUNIT_H

#include <stdio.h>
#include <string.h>

/* Test statistics */
static int mu_tests_run = 0;
static int mu_tests_failed = 0;
static int mu_assertions = 0;

/* Current test name */
static const char *mu_current_test = NULL;

/* Message buffer for failure reporting */
static char mu_last_message[1024];

/**
 * @brief Run a test suite
 * @param name Name of the test suite
 * @param test_func Function containing the tests
 */
#define mu_suite(name) static void name(void)

/**
 * @brief Define a single test
 * @param name Test name
 */
#define mu_test(name) \
    void name(void); \
    void name(void)

/**
 * @brief Assert that expression is true
 * @param message Message to display on failure
 * @param expression Expression to evaluate
 */
#define mu_assert(message, expression) \
    do { \
        mu_assertions++; \
        if (!(expression)) { \
            snprintf(mu_last_message, sizeof(mu_last_message), \
                     "%s:%d: %s - %s", \
                     __FILE__, __LINE__, message, #expression); \
            return; \
        } \
    } while (0)

/**
 * @brief Assert that two integers are equal
 * @param expected Expected value
 * @param actual Actual value
 */
#define mu_assert_eq_int(expected, actual) \
    do { \
        mu_assertions++; \
        if ((expected) != (actual)) { \
            snprintf(mu_last_message, sizeof(mu_last_message), \
                     "%s:%d: Expected %d but got %d", \
                     __FILE__, __LINE__, (int)(expected), (int)(actual)); \
            return; \
        } \
    } while (0)

/**
 * @brief Assert that pointer is not NULL
 * @param ptr Pointer to check
 */
#define mu_assert_not_null(ptr) \
    mu_assert(#ptr " should not be NULL", (ptr) != NULL)

/**
 * @brief Assert that pointer is NULL
 * @param ptr Pointer to check
 */
#define mu_assert_null(ptr) \
    mu_assert(#ptr " should be NULL", (ptr) == NULL)

/**
 * @brief Run a single test function
 * @param test_func Test function to run
 */
#define mu_run_test(test_func) \
    do { \
        mu_current_test = #test_func; \
        mu_last_message[0] = '\0'; \
        test_func(); \
        mu_tests_run++; \
        if (mu_last_message[0] != '\0') { \
            mu_tests_failed++; \
            printf("  FAILED: %s\n    %s\n", #test_func, mu_last_message); \
        } else { \
            printf("  PASSED: %s\n", #test_func); \
        } \
    } while (0)

/**
 * @brief Print test summary
 */
#define mu_print_summary() \
    do { \
        printf("\n========================================\n"); \
        printf("Tests run: %d\n", mu_tests_run); \
        printf("Tests passed: %d\n", mu_tests_run - mu_tests_failed); \
        printf("Tests failed: %d\n", mu_tests_failed); \
        printf("Assertions: %d\n", mu_assertions); \
        printf("========================================\n"); \
    } while (0)

/**
 * @brief Return appropriate exit code
 * @return 0 if all tests passed, 1 otherwise
 */
#define mu_return_status() (mu_tests_failed == 0 ? 0 : 1)

/**
 * @brief Reset test counters (for multiple test files)
 */
#define mu_reset() \
    do { \
        mu_tests_run = 0; \
        mu_tests_failed = 0; \
        mu_assertions = 0; \
    } while (0)

#endif /* MINUNIT_H */
