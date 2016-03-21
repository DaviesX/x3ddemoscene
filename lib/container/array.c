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


//
//
//
//#define cmp_mid( _mval, _elm )          (*(_mval) > *(_elm))
///* alg_split_bymidvalue algorithm correctness test */
//static void split_by_midvalue_c ( struct alg_named_params *param )
//{
//        const int n = 100;
//        int *list = alloc_fix ( sizeof ( int ), n );
//        srand ( time ( nullptr ) );
//        int i;
//        for ( i = 0; i < n; i ++ ) {
//                list[i] = rand ()%n + 1;
//        }
//        int mid_value = n >> 1;
//        int mid_pos;
//        alg_split_bymidvalue ( list, n, &mid_value, cmp_mid, &mid_pos );
//        for ( i = 0; i < n; i ++ ) {
//                log_normal ( "%d: %d", i, list[i] );
//        }
//        log_normal ( "mid_value: %d", mid_value );
//        log_normal ( "mid_pos: %d, %d", mid_pos, list[mid_pos] );
//
//        int failed = 0;
//        for ( i = 0; i < mid_pos; i ++ ) {
//                if ( list[i] >= mid_value ) {
//                        log_normal ( "Lower half FAILED !" );
//                        log_normal ( "%d %d", i, list[i] );
//                        failed = 1;
//                }
//        }
//        for ( i = mid_pos; i < n; i ++ ) {
//                if ( list[i] < mid_value ) {
//                        log_normal ( "Greater half FAILED !" );
//                        log_normal ( "%d: %d", i, list[i] );
//                        failed = 1;
//                }
//        }
//        if ( !failed ) {
//                log_normal ( "ALL CORRECT" );
//        }
//        free_fix ( list );
//}
//
///* alg_split_bymidvalue algorithm performance test */
//static void split_by_midvalue_p ( struct alg_named_params *param )
//{
//        const int n = 10000000;
//        int *list = alloc_fix ( sizeof ( int ), n );
//        int begin = clock ();
//        int k;
//        for ( k = 0; k < 20; k ++ ) {
//                int i;
//                for ( i = 0; i < n; i ++ ) {
//                        list[i] = rand ()%n + 1;
//                }
//                int mid_pos;
//                int mid_value = n >> 1;
//                alg_split_bymidvalue ( list, n, &mid_value, cmp_mid, &mid_pos );
//        }
//        int end = clock ();
//        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
//        free_fix ( list );
//}
//#undef cmp_mid
//
///* alg_split_byindex algorithm correctness test */
//static void split_by_index_c ( struct alg_named_params *param )
//{
//        const int n = 1000;
//        int *list = alloc_fix ( sizeof ( int ), n );
//        srand ( time ( nullptr ) );
//        int index;
//        for ( index = 0; index < n; index ++ ) {
//                list[index] = rand ()%n + 1;
//        }
//        alg_split_byindex ( list, n, 1000, nullptr, cmp_int );
//        log_normal ( "Nth is: %d, %d\n", 1000, list[n >> 1] );
////        std::nth_element ( list, list + 1000, list + n );
////        log_normal ( "Nth is: %d, %d\n", 1000, list[n >> 1] );
//        free_fix ( list );
//}
//
///* alg_split_byindex algorithm performance test */
//static void split_by_index_p ( struct alg_named_params *param )
//{
//        const int n = 10000000;
//        int *list = alloc_fix ( sizeof ( int ), n );
//        int begin = clock ();
//        int k;
//        for ( k = 0; k < 20; k ++ ) {
//                int index;
//                for ( index = 0; index < n; index ++ ) {
//                        list[index] = rand ()%n + 1;
//                }
//                alg_split_byindex ( list, n, n >> 1, nullptr, cmp_int );
//        }
//        int end = clock ();
//        log_normal ( "Nth is: %d, %d\n", n >> 1, list[n >> 1] );
//        log_normal ( "Time: %f\n", (float) (end - begin)/CLOCKS_PER_SEC );
//        free_fix ( list );
//}
