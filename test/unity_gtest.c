/* unity_gtest.c — GTest-style output plugin for Unity
 *
 * Hooks:
 *   UNITY_EXEC_TIME_START  → prints "[ RUN      ] name" before each test
 *   UNITY_EXEC_TIME_STOP   → computes elapsed; prints "[  FAILED  ]" on failure
 *   UNITY_OUTPUT_CHAR      → buffers Unity's raw output line-by-line and
 *                            reformats:
 *                              file:line:name:PASS  → "[       OK ] name (N ms)"
 *                              file:line:name:FAIL  → (handled in exec_time_stop)
 *                              file:line:name:IGNORE→ "[ SKIPPED  ] name"
 *                              summary line         → "[==========] / [  PASSED  ]"
 *                              "OK" / "FAIL"        → suppressed (redundant)
 */
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "unity.h"

/* ---- ANSI colour codes -------------------------------------------------- */
#define GRN "\033[0;32m"
#define RED "\033[0;31m"
#define YEL "\033[0;33m"
#define BOLD "\033[1m"
#define RST "\033[0m"

/* ---- module state ------------------------------------------------------- */
static struct timespec s_t0;
static long s_elapsed_ms;

/* line buffer for UNITY_OUTPUT_CHAR */
static char s_buf[1024];
static int s_pos;

/* failure details buffered during test execution */
static char s_fail_loc[256]; /* "file:line" */
static char s_fail_msg[512]; /* message after FAIL: */
static int s_has_fail;

/* ---- helpers ------------------------------------------------------------ */

/*
 * Fill cp[0..3] with the positions of the first four colons in `line`.
 * Returns the number of colons found (capped at 4).
 */
static int find_colons(const char *line, int cp[4]) {
    int n = 0;
    cp[0] = cp[1] = cp[2] = cp[3] = -1;
    for (int i = 0; line[i] && n < 4; i++)
        if (line[i] == ':') cp[n++] = i;
    return n;
}

/* ---- line reformatter --------------------------------------------------- */

static void handle_line(const char *line) {
    if (line[0] == '\0') return; /* suppress blank lines */

    int cp[4], nc = find_colons(line, cp);

    /* --- test result lines: file:line:name:PASS/FAIL/IGNORE -------------- */
    if (nc >= 3) {
        const char *result = line + cp[2] + 1;
        int name_len       = cp[2] - cp[1] - 1;
        const char *name   = line + cp[1] + 1;

        if (strncmp(result, "PASS", 4) == 0) {
            printf(GRN "[       OK ]" RST " %.*s (%ld ms)\n", name_len, name, s_elapsed_ms);
            return;
        }
        if (strncmp(result, "FAIL", 4) == 0) {
            /* Buffer location and message; printed in gtest_exec_time_stop. */
            snprintf(s_fail_loc, sizeof(s_fail_loc), "%.*s", cp[1], line);
            const char *msg = (nc >= 4) ? line + cp[3] + 1 : "";
            snprintf(s_fail_msg, sizeof(s_fail_msg), "%s", msg);
            s_has_fail = 1;
            return;
        }
        if (strncmp(result, "IGNORE", 6) == 0) {
            printf(YEL "[ SKIPPED  ]" RST " %.*s\n", name_len, name);
            return;
        }
    }

    /* --- Unity separator line -------------------------------------------- */
    if (strncmp(line, "-----------------------", 23) == 0) return;

    /* --- summary: "N Tests N Failures N Ignored" (Unity v2.6 format) ----- */
    if (strstr(line, " Tests ")) {
        int tests = 0, fails = 0, ignored = 0;
        /* May be prefixed with "file:"; skip to the digits if so. */
        const char *p = (nc >= 1) ? line + cp[0] + 1 : line;
        sscanf(p, "%d Tests %d Failures %d Ignored", &tests, &fails, &ignored);
        int passed = tests - fails - ignored;
        printf(GRN "[==========]" RST " %d test%s ran.\n", tests, tests != 1 ? "s" : "");
        if (ignored > 0)
            printf(YEL "[ SKIPPED  ]" RST " %d test%s.\n", ignored, ignored != 1 ? "s" : "");
        if (fails == 0)
            printf(GRN "[  PASSED  ]" RST " %d test%s.\n", passed, passed != 1 ? "s" : "");
        else
            printf(RED "[  FAILED  ]" RST " %d test%s.\n", fails, fails != 1 ? "s" : "");
        return;
    }

    /* --- final "OK" / "FAIL" line — already covered by summary above ----- */
    if (strcmp(line, "OK") == 0 || strcmp(line, "FAIL") == 0) return;

    /* --- anything else: pass through ------------------------------------- */
    printf("%s\n", line);
}

/* ---- UNITY_OUTPUT_CHAR -------------------------------------------------- */

void gtest_output_char(char c) {
    if (c == '\n') {
        s_buf[s_pos] = '\0';
        handle_line(s_buf);
        s_pos = 0;
    } else if (s_pos < (int)sizeof(s_buf) - 1) {
        s_buf[s_pos++] = c;
    }
}

void gtest_output_flush(void) { fflush(stdout); }

/* ---- UNITY_OUTPUT_START ------------------------------------------------- */

void gtest_output_start(void) {
    /* Extract suite name from TestFile (basename without extension). */
    const char *file  = Unity.TestFile ? Unity.TestFile : "";
    const char *slash = strrchr(file, '/');
    if (slash) file = slash + 1;
    char suite[64];
    strncpy(suite, file, sizeof(suite) - 1);
    suite[sizeof(suite) - 1] = '\0';
    char *dot                = strrchr(suite, '.');
    if (dot) *dot = '\0';

    printf("\n" GRN BOLD "[----------]" RST " %s\n", suite);
    fflush(stdout);
}

/* ---- UNITY_EXEC_TIME_START ---------------------------------------------- */

void gtest_exec_time_start(void) {
    s_has_fail    = 0;
    s_fail_loc[0] = '\0';
    s_fail_msg[0] = '\0';
    clock_gettime(CLOCK_MONOTONIC, &s_t0);
    printf(GRN "[ RUN      ]" RST " %s\n", Unity.CurrentTestName);
    fflush(stdout);
}

/* ---- UNITY_EXEC_TIME_STOP ----------------------------------------------- */

void gtest_exec_time_stop(void) {
    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    s_elapsed_ms = (t1.tv_sec - s_t0.tv_sec) * 1000L + (t1.tv_nsec - s_t0.tv_nsec) / 1000000L;

    /* PASS: s_elapsed_ms is now set; the "[       OK ]" line is printed
     * later by handle_line() when UnityConcludeTest emits the PASS line. */
    if (!Unity.CurrentTestFailed) return;

    /* FAIL: print buffered location + message, then the banner. */
    if (s_has_fail) {
        printf(RED "  %s: Failure" RST "\n", s_fail_loc);
        if (s_fail_msg[0]) printf("    %s\n", s_fail_msg);
    }
    printf(RED "[  FAILED  ]" RST " %s (%ld ms)\n", Unity.CurrentTestName, s_elapsed_ms);
}
