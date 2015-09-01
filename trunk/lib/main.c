/* main.c: All unit tests for lib module go here */
#include <system/log.h>
#include <system/allocator.h>
#include <container/paramset.h>
#include <container/staging.h>
#include <container/quicksort.h>
#include <system/thread.h>
#include <system/timing.h>
#include <x3d/debug.h>
#include "main.h"


static void variable_memory_test0 ( struct alg_named_params *param );
static void variable_memory_test1 ( struct alg_named_params *param );

static void error_report ( struct alg_named_params *param );
static void quick_sort_c ( struct alg_named_params *param );
static void quick_sort_p ( struct alg_named_params *param );

static void split_by_midvalue_c ( struct alg_named_params *param );
static void split_by_midvalue_p ( struct alg_named_params *param );
static void split_by_index_c ( struct alg_named_params *param );
static void split_by_index_p ( struct alg_named_params *param );

static void ptr_list_c ( struct alg_named_params *param );

static void hash_container_c ( struct alg_named_params *param );
static void hash_container_p ( struct alg_named_params *param );

static void llist_container_c ( struct alg_named_params *param );

void variable_memory_test0 ( struct alg_named_params *param )
{
        char **str_arr = alloc_var ( sizeof ( char* ), 0 );
        char *s0 = "Hello, first string\n";
        str_arr = alloc_push_var ( &s0, str_arr );
        char *s1 = "Do you see me ? Second string\n";
        str_arr = alloc_push_var ( &s1, str_arr );
        log_normal ( "%s", &str_arr[0][0] );
        log_normal ( "%s", &str_arr[1][0] );
        free_var ( str_arr );
//	dbg_finish ();
}

void variable_memory_test1 ( struct alg_named_params *param )
{
        log_normal ( "memory query 0: %dkb", alloc_query_usage () );
        const int count = 10000;
        /* add/decrease */
        int *array = alloc_var ( sizeof ( int ), 0 );
        log_normal ( "memory query 1: %dkb", alloc_query_usage () );
        int i;
        for ( i = 0; i < count; i ++ ) {
                array = alloc_add_var ( array, 1 );
                array[i] = i;

//                int *curr = get_var_last ( array );
                set_log_behavior ( LOG_OUTPUT_TO_FILE );
                log_normal ( "%d", array[i] );
        }
        log_normal ( "number element(added): %d", get_var_len ( array ) );
        for ( i = 0; i < count/2; i ++ ) {
                array = dec_var ( array, 2 );
        }
        log_normal ( "number element(deleted): %d", get_var_len ( array ) );
        log_normal ( "memory query 2: %dkb", alloc_query_usage () );
        /* shrink-expand */
        array = shrink_var ( array );
        log_normal ( "number element(shrinked): %d", get_var_len ( array ) );
        log_normal ( "memory query 3: %dkb", alloc_query_usage () );
        for ( i = 0; i < count; i ++ ) {
                array = alloc_expand_var ( array, i + 1 );
                array[i] = i;
        }
        log_normal ( "number element(expanded): %d", get_var_len ( array ) );
        flush_var ( array );
        log_normal ( "number element(flushed): %d", get_var_len ( array ) );
        free_var ( array );
        log_normal ( "memory query 4: %dkb", alloc_query_usage () );
//	dbg_finish ();
}

static void error_report ( struct alg_named_params *param )
{
        /* Test the error report code in logout.h/c */
        log_init ( 0 );
        log_normal ( "This is a normal message" );
        log_mild_err ( "This is a mild error message" );
        log_severe_err ( "This is a severe error message" );
        log_critical_err ( "This is a critical error message" );
        int para = 10, para1 = 15;
        set_log_behavior ( LOG_OUTPUT_TO_FILE );
        log_normal ( "There are two numbers: %d %d", para, para1 );
}

#define cmp_int( _n0, _n1, _info )    ((_n0) < (_n1))

/* alg_quick_sort algorithm correctness test */
static void quick_sort_c ( struct alg_named_params *param )
{
        const int n = 100;
        int list[n];
        int i;
        for ( i = 0; i < n; i ++ ) {
                list[i] = rand ()%n + 1;
                log_normal ( "%d", list[i] );
        }
        alg_quick_sort ( list, n, nullptr, cmp_int );
        log_normal ( "" );
        for ( i = 0; i < n - 1; i ++ ) {
                if ( list[i + 1] < list[i] ) {
                        log_normal ( "Failed" );
                }
        }

}

/* alg_quick_sort algorithm performance test */
static void quick_sort_p ( struct alg_named_params *param )
{
        const int n = 10000000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int begin = clock ();
        int k;
        for ( k = 0; k < 5; k ++ ) {
                int i;
                for ( i = 0; i < n; i ++ ) {
                        list[i] = rand ();
                }
                alg_quick_sort ( list, n, nullptr, cmp_int );
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        free_fix ( list );
}

#define cmp_mid( _mval, _elm )          (*(_mval) > *(_elm))
/* alg_split_bymidvalue algorithm correctness test */
static void split_by_midvalue_c ( struct alg_named_params *param )
{
        const int n = 100;
        int *list = alloc_fix ( sizeof ( int ), n );
        srand ( time ( nullptr ) );
        int i;
        for ( i = 0; i < n; i ++ ) {
                list[i] = rand ()%n + 1;
        }
        int mid_value = n >> 1;
        int mid_pos;
        alg_split_bymidvalue ( list, n, &mid_value, cmp_mid, &mid_pos );
        for ( i = 0; i < n; i ++ ) {
                log_normal ( "%d: %d", i, list[i] );
        }
        log_normal ( "mid_value: %d", mid_value );
        log_normal ( "mid_pos: %d, %d", mid_pos, list[mid_pos] );

        int failed = 0;
        for ( i = 0; i < mid_pos; i ++ ) {
                if ( list[i] >= mid_value ) {
                        log_normal ( "Lower half FAILED !" );
                        log_normal ( "%d %d", i, list[i] );
                        failed = 1;
                }
        }
        for ( i = mid_pos; i < n; i ++ ) {
                if ( list[i] < mid_value ) {
                        log_normal ( "Greater half FAILED !" );
                        log_normal ( "%d: %d", i, list[i] );
                        failed = 1;
                }
        }
        if ( !failed ) {
                log_normal ( "ALL CORRECT" );
        }
        free_fix ( list );
}

/* alg_split_bymidvalue algorithm performance test */
static void split_by_midvalue_p ( struct alg_named_params *param )
{
        const int n = 10000000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int begin = clock ();
        int k;
        for ( k = 0; k < 20; k ++ ) {
                int i;
                for ( i = 0; i < n; i ++ ) {
                        list[i] = rand ()%n + 1;
                }
                int mid_pos;
                int mid_value = n >> 1;
                alg_split_bymidvalue ( list, n, &mid_value, cmp_mid, &mid_pos );
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        free_fix ( list );
}
#undef cmp_mid

/* alg_split_byindex algorithm correctness test */
static void split_by_index_c ( struct alg_named_params *param )
{
        const int n = 1000;
        int *list = alloc_fix ( sizeof ( int ), n );
        srand ( time ( nullptr ) );
        int index;
        for ( index = 0; index < n; index ++ ) {
                list[index] = rand ()%n + 1;
        }
        alg_split_byindex ( list, n, 1000, nullptr, cmp_int );
        log_normal ( "Nth is: %d, %d\n", 1000, list[n >> 1] );
//        std::nth_element ( list, list + 1000, list + n );
//        log_normal ( "Nth is: %d, %d\n", 1000, list[n >> 1] );
        free_fix ( list );
}

/* alg_split_byindex algorithm performance test */
static void split_by_index_p ( struct alg_named_params *param )
{
        const int n = 10000000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int begin = clock ();
        int k;
        for ( k = 0; k < 20; k ++ ) {
                int index;
                for ( index = 0; index < n; index ++ ) {
                        list[index] = rand ()%n + 1;
                }
                alg_split_byindex ( list, n, n >> 1, nullptr, cmp_int );
        }
        int end = clock ();
        log_normal ( "Nth is: %d, %d\n", n >> 1, list[n >> 1] );
        log_normal ( "Time: %f\n", (float) (end - begin)/CLOCKS_PER_SEC );
        free_fix ( list );
}

/* alg_ptr_list algorithm correctness test */
static void ptr_list_c ( struct alg_named_params *param )
{
#define cmp_ptrs( _n0, _n1, _info )             (*(int *) ((_n0)->ptr) < *(int *) ((_n1)->ptr))
        bool is_inplace = true;
        bool failed = false;

        const int n = 1000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int i;
        for ( i = 0; i < n; i ++ ) {
                list[i] = rand ()%n + 1;
                log_normal ( "%d", list[i] );
        }

        if ( is_inplace ) {
                struct alg_ptr_list ptrs;
                alg_ptr_list_init(&ptrs, list, n, is_inplace);
                alg_quick_sort(&ptrs, n, nullptr, cmp_ptrs);
                alg_ptr_list_inplace(&ptrs, n, list);

                log_normal ( "in-place result: " );
                for ( i = 0; i < n - 1; i ++ ) {
                        log_normal ( "%d", list[i] );
                        if ( list[i + 1] < list [i] ) {
                                failed = true;
                        }
                }
                free_fix ( list );
        } else {
                int *new_list = alloc_fix ( sizeof ( int ), n );
                struct alg_ptr_list ptrs;
                alg_ptr_list_init(&ptrs, list, n, is_inplace);
                struct alg_ptr_list *list = &ptrs;
                alg_quick_sort ( list, n, nullptr, cmp_ptrs );
                alg_ptr_list_tonew ( &ptrs, n, new_list );

                puts ( "non-in-place result: " );
                for ( i = 0; i < n - 1; i ++ ) {
                        log_normal ( "%d", new_list[i] );
                        if ( new_list[i + 1] < new_list [i] ) {
                                failed = true;
                        }
                }
                free_fix ( new_list );
                free_fix ( list );
        }
        if ( failed ) {
                log_normal ( "FAILED" );
        }
#undef cmp_ptrs
}

#undef cmp_int

#define hash_cmp_val( _info, _elm )             (*(_info) == *(_elm))
static void hash_container_c ( struct alg_named_params *param )
{
        int array[11] = {0, 4, 2, 3, 4, 5, 10, 2, 0, 3, 5};

        struct alg_hash_llist hash_llist;
        alg_hash_llist_init(&hash_llist, 1 << 8, 0, sizeof (int));
        int i;
        for ( i = 0; i < 11; i ++ ) {
                log_normal ( "Looking up..." );
                int *rfound = nullptr;
                alg_hash_llist_first ( &hash_llist, i, &rfound, &array[i], hash_cmp_val );

                if ( rfound != nullptr ) {
                        log_normal ( "index(%d), data(%d)\nindex(%d), data(%d)\n",
                                     rfound - array, *rfound, i, array[i] );
                } else {
                        log_normal ( "couldn't find such element: index(%d), data(%d). it is now inserted.", i, array[i] );
                        alg_hash_llist_add ( &hash_llist, &array[i], i, f_Dym_Update );
                }
        }
        alg_hash_llist_free(&hash_llist);

}

static void hash_container_p ( struct alg_named_params *param )
{
        const int n = 20000;
        int array[n];
        int i;
        for ( i = 0; i < n; i ++ ) {
                array[i] = rand ()%n;
        }

        struct alg_hash_llist hash_llist;
        alg_hash_llist_init(&hash_llist, 1 << 15, 1, sizeof (int));
        int begin = clock ();
        int k;
        for ( k = 0; k < 10000; k ++ ) {
                for ( i = 0; i < n; i ++ ) {
                        int *rfound = nullptr;
                        alg_hash_llist_first (
                                &hash_llist, gen_hash32 (array[i]), &rfound,
                                &array[i], hash_cmp_val );

                        if ( rfound == nullptr ) {
                                alg_hash_llist_add ( &hash_llist, &array[i],
                                                     gen_hash32 (array[i]), f_Dym_Update );
                        }
                }
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        alg_hash_llist_free(&hash_llist);
}

static void llist_container_c ( struct alg_named_params *param )
{
        bool failed = false;
        d_alg_llist(int) llist;
        alg_init ( llist, &llist, sizeof (int), 0 );

        const int n = 10;
        int array[n];
        int i;
        for ( i = 0; i < n; i ++ ) {
                array[i] = rand() % n;
        }

        for ( i = 0; i < n; i ++ ) {
                alg_push_back ( llist, &llist, array[i] );
        }

#define cmp_llist( _data, _iter )                (_data == alg_access(_iter))
        for ( i = 0; i < n; i ++ ) {
                alg_iter(int) x;
                alg_find ( llist, &llist, array[i], x, cmp_llist );
                if ( x != nullptr ) {
                        if ( *x != array[i] ) {
                                log_normal ( "x != array[i]" );
                                failed = true;
                        }
                } else {
                        log_normal ( "content couldn't find" );
                        failed = true;
                }
        }
#undef cmp_llist
        alg_free ( llist, &llist );
        if ( !failed ) {
                log_normal ( "llist_container_c stage 1 passed" );
        } else {
                log_normal ( "llist_container_c stage 1 failed" );
        }

        failed = false;
        alg_init ( llist, &llist, sizeof(int), 1 );
        for ( i = 0; i < n; i ++ ) {
                alg_push_front ( llist, &llist, array[i] );
        }

        for ( i = 0; i < n; i ++ ) {
                alg_iter(int) x;
                alg_last ( llist, &llist, x );
                if ( x != nullptr ) {
                        if ( alg_access(x) != array[n - i - 1] ) {
                                log_normal ( "x != array[n - i]" );
                                failed = true;
                        }
                } else {
                        log_normal ( "over popped" );
                        failed = true;
                }
                alg_pop_back ( llist, &llist, x );
        }
        alg_free ( llist, &llist );

        if ( !failed ) {
                log_normal ( "llist_container_c stage 2 passed" );
        } else {
                log_normal ( "llist_container_c stage 2 failed" );
        }
}

#undef hash_cmp_val
