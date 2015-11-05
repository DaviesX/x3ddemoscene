#include <x3d/common.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static bool     g_ignore_panic = false;

static void __print_backtrace(int sig) 
{
        void*  array[64];
        size_t size = backtrace(array, 64);
        fprintf(stderr, ANSI_COLOR_RED"X3d experienced critical error: signal %d. program exit with backtrace:\n\n", sig);
        backtrace_symbols_fd(array, size, STDERR_FILENO);
        exit(1);
}

void panic_init(bool ignore_panic)
{
        signal(SIGSEGV, __print_backtrace);
        signal(SIGABRT, __print_backtrace);
        g_ignore_panic = ignore_panic;
}

void panic()
{
        if (!g_ignore_panic) {
                puts("kernel panic, will now exit");
                abort();
        } else {
                puts("critical error triggered, but ignored");
        }
}
