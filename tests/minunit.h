/**
 * @file minunit.h
 * @brief Minimalistisches Unit-Test Framework für C (Single Header)
 * 
 * Dies ist eine modifizierte Version von minunit (https://github.com/siu/minunit)
 * Lizenziert unter MIT License
 */

#ifndef MINUNIT_H
#define MINUNIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Zeigt Fehler an und zählt fehlgeschlagene Tests
 */
#define mu_fail(message) do { \
    fprintf(stderr, "FAILED: %s:%d: %s\n", __FILE__, __LINE__, message); \
    minunit_fail++; \
    return message; \
} while (0)

/**
 * @brief Prüft ob Bedingung wahr ist
 */
#define mu_assert(message, test) do { \
    if (!(test)) { \
        fprintf(stderr, "FAILED: %s:%d: %s - %s\n", __FILE__, __LINE__, message, #test); \
        minunit_fail++; \
        return message; \
    } \
} while (0)

/**
 * @brief Prüft ob zwei Ganzzahlen gleich sind
 */
#define mu_assert_int_eq(expected, actual) do { \
    int minunit_tmp_expected = (expected); \
    int minunit_tmp_actual = (actual); \
    if (minunit_tmp_expected != minunit_tmp_actual) { \
        fprintf(stderr, "FAILED: %s:%d: %d expected but got %d\n", \
                __FILE__, __LINE__, minunit_tmp_expected, minunit_tmp_actual); \
        minunit_fail++; \
        return "Integer comparison failed"; \
    } \
} while (0)

/**
 * @brief Prüft ob zwei Zeichenketten gleich sind
 */
#define mu_assert_string_eq(expected, actual) do { \
    const char* minunit_tmp_expected = (expected); \
    const char* minunit_tmp_actual = (actual); \
    if ((minunit_tmp_expected == NULL && minunit_tmp_actual != NULL) || \
        (minunit_tmp_expected != NULL && minunit_tmp_actual == NULL) || \
        (minunit_tmp_expected != NULL && minunit_tmp_actual != NULL && \
         strcmp(minunit_tmp_expected, minunit_tmp_actual) != 0)) { \
        fprintf(stderr, "FAILED: %s:%d: \"%s\" expected but got \"%s\"\n", \
                __FILE__, __LINE__, \
                minunit_tmp_expected ? minunit_tmp_expected : "(null)", \
                minunit_tmp_actual ? minunit_tmp_actual : "(null)"); \
        minunit_fail++; \
        return "String comparison failed"; \
    } \
} while (0)

/**
 * @brief Startet einen Test
 */
#define mu_run_test(test) do { \
    const char* message = test(); \
    minunit_run++; \
    if (message) { \
        printf("  FAILED: %s\n", message); \
    } else { \
        printf("  PASSED: %s\n", #test); \
    } \
} while (0)

/**
 * @brief Startet die Test-Suite
 */
#define mu_suite_start() do { \
    minunit_run = 0; \
    minunit_fail = 0; \
} while (0)

/**
 * @brief Zeigt Test-Zusammenfassung an
 */
#define mu_suite_report() do { \
    printf("\n"); \
    printf("========================================\n"); \
    printf("Tests run: %d\n", minunit_run); \
    printf("Tests passed: %d\n", minunit_run - minunit_fail); \
    printf("Tests failed: %d\n", minunit_fail); \
    printf("========================================\n"); \
    return minunit_fail; \
} while (0)

/* Globale Zähler */
static int minunit_run = 0;
static int minunit_fail = 0;

#endif /* MINUNIT_H */
