#include <container/set.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>


__dlexport void __callback                  set_correctness_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  set_correctness_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   set_correctness_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  set_correctness_test(struct alg_var_set *envir)
{
        /*const int n = 11;
        int array[11] = {0, 4, 2, 3, 4, 5, 10, 2, 0, 3, 5};*/
        int i;
        const int n = 200;
        int array[n];
        for ( i = 0; i < n; i ++ ) {
                array[i] = rand ()%n;
        }

        set_templ(int, void) set;
        set_iter_templ(int) iterator;
        set_init(&set, 1, nullptr);

        #define int_equal(a, b, data)         (*(a) == *(b))
        log_normal("testing add...");
        for (i = 0; i < n; i ++) {
                set_add(&set, array[i], int_hash, int_equal, &iterator);
        }

        log_normal("testing iterator and verifying result...");
        int test[n];
        i = 0;
        set_iter_begin(&iterator, &set);
        while (set_iter_has_next(&iterator, &set)) {
                int e = set_iter_deref(&iterator, &set);
                int j;
                for (j = 0; j < i; j ++) {
                        if (test[j] == e) {
                                log_severe_err("value: %d is not unique", e);
                        }
                }
                test[i ++] = e;
                log_normal("value: %d", e);
                set_iter_next(&iterator, &set);
        }
        log_normal("verifying result...");
        for (i = 0; i < n; i ++) {
                set_find(&set, array[i], int_hash, int_equal, &iterator);
                if (!set_iter_has_next(&iterator, &set)) {
                        log_severe_err("missing value: %d", array[i]);
                }
        }

        if (set_size(&set) <= 0 || set_size(&set) > n) {
                log_severe_err("size is incorrect after add");
        }
        set_flush(&set);
        if (set_size(&set) != 0) {
                log_severe_err("size is not zero after flush");
        }

        log_normal("testing find and add...");
        for (i = 0; i < n; i ++) {
                set_find(&set, array[i], int_hash, int_equal, &iterator);
                if (set_iter_has_next(&iterator, &set)) {
                        log_normal("found: %d, at %d", set_iter_deref(&iterator, &set), i);
                } else {
                        log_normal("couldn't find such element: index(%d), data(%d). it is now inserted.", i, array[i]);
                        set_add(&set, array[i], int_hash, int_equal, &iterator);
                }
        }
        log_normal("verifying result...");
        for (i = 0; i < n; i ++) {
                set_find(&set, array[i], int_hash, int_equal, &iterator);
                if (!set_iter_has_next(&iterator, &set)) {
                        log_severe_err("missing value: %d", array[i]);
                }
        }

        log_normal("testing removal...");
        for (i = 0; i < n; i ++) {
                set_find(&set, array[i], int_hash, int_equal, &iterator);
                if (set_iter_has_next(&iterator, &set)) {
                        set_remove_at(&set, &iterator);
                }
        }
        log_normal("verifying result...");
        for (i = 0; i < n; i ++) {
                set_find(&set, array[i], int_hash, int_equal, &iterator);
                if (set_iter_has_next(&iterator, &set)) {
                        log_severe_err("value still present: %d", array[i]);
                }
        }
        #undef int_equal
        set_free(&set);

}

__dlexport void __callback                  set_performance_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  set_performance_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   set_performance_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void set_performance_test(struct alg_var_set *envir)
{
        const int n = 20000;
        int array[n];
        int i;
        for ( i = 0; i < n; i ++ ) {
                array[i] = rand ()%n;
        }

        set_templ(int, void) set;
        set_iter_templ(int) iterator;
        set_init(&set, 1, nullptr);

        int k;
        for (k = 0; k < 10000; k ++) {
                #define int_equal(a, b, data)         (*(a) == *(b))
                for (i = 0; i < n; i ++) {
                        set_add(&set, array[i], int_hash, int_equal, &iterator);
                }
                for (i = 0; i < n; i ++) {
                        set_find(&set, array[i], int_hash, int_equal, &iterator);
                        if (!set_iter_has_next(&iterator, &set)) {
                                log_severe_err("missing value: %d", array[i]);
                        }
                }
                #undef int_equal
        }
        set_free(&set);
}
