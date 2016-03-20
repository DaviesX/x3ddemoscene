#include <x3d/common.h>
#include <system/log.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static bool     g_ignore_panic = false;

static void __print_backtrace(int sig)
{
        void*  array[64];
        size_t size = backtrace(array, 64);
        log_severe_err("X3d experienced critical error: signal %d. program exit with backtrace:\n\n", sig);
        char** symbols = backtrace_symbols(array, size);
        int i;
        for (i = 0; i < size; i ++) {
                log_severe_err("%s", symbols[i]);
        }
        free(symbols);
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
