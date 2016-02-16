/* main.c: All unit tests for lib module go here */
#include <system/log.h>
#include <system/allocator.h>
#include <container/paramset.h>
#include <container/staging.h>
#include <container/quicksort.h>
#include <system/thread.h>
#include <system/timing.h>
#include <x3d/common.h>
#include <x3d/debug.h>
#include "main.h"

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

#undef hash_cmp_val
