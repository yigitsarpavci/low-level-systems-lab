/*
 * Automated tests for the IPC subsystem: ring buffer, signal delivery,
 * FIFO command decoding, dynamic loading of the FIFO library and dispatch().
 * Dependency-free: no test framework, exits non-zero on the first failing group.
 *
 * Build and run with `make test` from the repository root.
 */
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE   /* mkdtemp on glibc */
#define _DARWIN_C_SOURCE  /* mkdtemp on macOS */

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "implementation/fifo_management.h"
#include "implementation/input_student.h"

#ifndef LIB_PATH
#error "LIB_PATH must point to libfifo_management.so (set by `make test`)"
#endif

static int checks = 0, failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        checks++;                                                                \
        if (!(cond)) {                                                           \
            failures++;                                                          \
            fprintf(stderr, "  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);    \
        }                                                                        \
    } while (0)

static void run(const char *name, void (*fn)(void)) {
    int before = failures;
    fn();
    printf("%-46s %s\n", name, failures == before ? "ok" : "FAILED");
}

/* Writes raw bytes into the game FIFO (current directory's game.fifo). */
static void fifo_write(const char *bytes) {
    int fd = open("game.fifo", O_WRONLY | O_NONBLOCK);
    CHECK(fd >= 0);
    if (fd >= 0) {
        CHECK(write(fd, bytes, strlen(bytes)) == (ssize_t)strlen(bytes));
        close(fd);
    }
}

/* ---------------------------------------------------------------- ring buffer */

static void test_ring_is_fifo_ordered(void) {
    enum input_op op;
    ring_init();
    CHECK(ring_pop(&op) == 0);
    ring_push(OP_UP);
    ring_push(OP_SHOOT);
    ring_push(OP_QUIT);
    CHECK(ring_count == 3);
    CHECK(ring_pop(&op) == 1 && op == OP_UP);
    CHECK(ring_pop(&op) == 1 && op == OP_SHOOT);
    CHECK(ring_pop(&op) == 1 && op == OP_QUIT);
    CHECK(ring_pop(&op) == 0);
}

static void test_ring_drops_when_full(void) {
    enum input_op op;
    ring_init();
    for (int i = 0; i < RING_SIZE + 10; i++)
        ring_push(OP_LEFT);
    CHECK(ring_count == RING_SIZE);
    int popped = 0;
    while (ring_pop(&op))
        popped++;
    CHECK(popped == RING_SIZE);
}

static void test_ring_wraps_around(void) {
    enum input_op op;
    ring_init();
    /* Advance head/tail past the end of the backing array several times. */
    for (int round = 0; round < 3 * RING_SIZE; round++) {
        ring_push((enum input_op)(round % OP_COUNT));
        CHECK(ring_pop(&op) == 1);
        CHECK(op == (enum input_op)(round % OP_COUNT));
    }
    CHECK(ring_count == 0);
}

/* -------------------------------------------------------------------- signals */

static void test_signals_become_ops(void) {
    enum input_op op;
    ring_init();
    CHECK(signal_register() == 0);
    raise(SIG_PAUSE);
    raise(SIG_QUIT);
    CHECK(ring_pop(&op) == 1 && op == OP_PAUSE);
    CHECK(ring_pop(&op) == 1 && op == OP_QUIT);
    CHECK(ring_pop(&op) == 0);
    signal_unregister();
}

/* ----------------------------------------------------------------------- FIFO */

static void test_fifo_decodes_commands(void) {
    CHECK(fifo_init() == 0);
    struct stat st;
    CHECK(stat("game.fifo", &st) == 0 && S_ISFIFO(st.st_mode));

    CHECK(fifo_read() == -1); /* nothing written yet: non-blocking, never EOF */

    fifo_write("udlrf");
    CHECK(fifo_read() == OP_UP);
    CHECK(fifo_read() == OP_DOWN);
    CHECK(fifo_read() == OP_LEFT);
    CHECK(fifo_read() == OP_RIGHT);
    CHECK(fifo_read() == OP_SHOOT);
    CHECK(fifo_read() == -1);

    fifo_write("sg");
    CHECK(fifo_read() == FIFO_SPAWN(OP_SHIELD_ACTIVATE));
    CHECK(fifo_read() == FIFO_SPAWN(OP_GATHER));

    char digit[2] = {FIFO_OP_CHAR(OP_QUIT), '\0'}; /* children write op digits */
    fifo_write(digit);
    CHECK(fifo_read() == OP_QUIT);

    fifo_write("zz?x\n"); /* unrecognized bytes are discarded */
    CHECK(fifo_read() == -1);

    CHECK(fifo_destroy() == 0);
    CHECK(stat("game.fifo", &st) != 0); /* file removed */
    CHECK(fifo_read() == -1);           /* closed descriptors */
}

static void test_fifo_survives_writer_disconnect(void) {
    CHECK(fifo_init() == 0);
    fifo_write("u"); /* writer opens, writes, closes */
    CHECK(fifo_read() == OP_UP);
    CHECK(fifo_read() == -1); /* O_RDWR keeps the pipe open: -1, not EOF handling */
    fifo_write("d");
    CHECK(fifo_read() == OP_DOWN);
    fifo_destroy();
}

/* -------------------------------------------------- dlopen + dispatch pipeline */

static void test_so_loading(void) {
    CHECK(so_load(NULL) == -1);
    CHECK(so_load("/nonexistent/libnope.so") == -1);
    CHECK(so_call(SO_SYM_INIT) == -1); /* nothing loaded */

    CHECK(so_load(LIB_PATH) == 0);
    CHECK(so_call("definitely_not_a_symbol") == -1);
    so_unload();
    CHECK(so_call(SO_SYM_INIT) == -1);
}

static void test_dispatch_drains_fifo_into_ring(void) {
    enum input_op op;
    ring_init();
    CHECK(so_load(LIB_PATH) == 0);
    CHECK(so_call(SO_SYM_INIT) == 0);

    fifo_write("ulf");
    dispatch();
    CHECK(ring_count == 3);
    CHECK(ring_pop(&op) == 1 && op == OP_UP);
    CHECK(ring_pop(&op) == 1 && op == OP_LEFT);
    CHECK(ring_pop(&op) == 1 && op == OP_SHOOT);

    so_call(SO_SYM_DESTROY);
    so_unload();
}

int main(void) {
    char tmpl[] = "/tmp/ipc-test-XXXXXX";
    char *dir = mkdtemp(tmpl);
    if (!dir || chdir(dir) != 0) {
        perror("mkdtemp/chdir");
        return 2;
    }

    printf("IPC subsystem tests (cwd %s)\n", dir);
    run("ring buffer keeps FIFO order", test_ring_is_fifo_ordered);
    run("ring buffer drops ops when full", test_ring_drops_when_full);
    run("ring buffer wraps around", test_ring_wraps_around);
    run("SIGUSR1/SIGTERM become PAUSE/QUIT ops", test_signals_become_ops);
    run("FIFO decodes commands, ignores noise", test_fifo_decodes_commands);
    run("FIFO survives writer disconnect (no EOF)", test_fifo_survives_writer_disconnect);
    run("dlopen loader validates library/symbols", test_so_loading);
    run("dispatch() drains FIFO into ring", test_dispatch_drains_fifo_into_ring);

    chdir("/");
    rmdir(dir);
    printf("\n%d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
