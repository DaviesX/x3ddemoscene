#include <container/array.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>


__dlexport void __callback                  array_sort_correctness_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  array_sort_correctness_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   array_sort_correctness_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  array_sort_correctness_test(struct alg_var_set *envir)
{
        const int n = 1000;
        int list[n];
        int i;
        for (i = 0; i < n; i ++) {
                list[i] = rand ()%n + 1;
                log_normal("sample: %d", list[i]);
        }
        array_templ(int, void) array;
        array_init(&array, list, list + n, nullptr);
#define cmp_int(a, b, data)    ((*a) < (*b))
        array_sort_unstable(&array, cmp_int);

        log_normal("");
        for (i = 0; i < n - 1; i ++) {
                if (list[i + 1] < list[i]) {
                        log_severe_err("Failed: %d, %d", list[i], list[i + 1]);
                }
        }
        int b, pivot = 50;
        array_bipartition(&array, pivot, cmp_int, &b);
        for (i = 0; i < b; i ++) {
                if (list[i] >= pivot) {
                        log_severe_err("bipartition failed");
                }
        }
#undef cmp_int
}

__dlexport void __callback                  array_sort_performance_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  array_sort_performance_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   array_sort_performance_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  array_sort_performance_test(struct alg_var_set* envir)
{
        const int n = 10000000;
        int *list = alloc_fix(sizeof(int), n);
        int begin = clock ();
        int k;
        for (k = 0; k < 5; k ++) {
                int i;
                for ( i = 0; i < n; i ++ ) {
                        list[i] = rand();
                }
                array_templ(int, void) array;
                array_init(&array, list, list + n, nullptr);
#define cmp_int(a, b, data)    ((*a) < (*b))
                array_sort_unstable(&array, cmp_int);
#undef cmp_int
        }
        int end = clock();
        log_normal("%f", (float) (end - begin)/CLOCKS_PER_SEC);
        free_fix(list);
}
