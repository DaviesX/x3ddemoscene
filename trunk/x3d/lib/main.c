/* main.c: All unit tests for lib module go here */
#include <logout.h>
#include <algorithm.h>
#include <memory.h>
#include <staging.h>
#include <thread.h>
#include <timing.h>
#include "main.h"



void variable_memory_test0 ( struct alg_named_params *param )
{
        char **str_arr = alloc_var ( sizeof ( char* ), 0 );
        char *s0 = "Hello, first string\n";
        str_arr = push_var ( &s0, str_arr );
        char *s1 = "Do you see me ? Second string\n";
        str_arr = push_var ( &s1, str_arr );
        log_normal ( "%s", &str_arr[0][0] );
        log_normal ( "%s", &str_arr[1][0] );
        free_var ( str_arr );
//	dbg_finish ();
}

void variable_memory_test1 ( struct alg_named_params *param )
{
        log_normal ( "memory query 0: %dkb", query_mem_usage () );
        const int count = 10000;
        /* add/decrease */
        int *array = alloc_var ( sizeof ( int ), 0 );
        log_normal ( "memory query 1: %dkb", query_mem_usage () );
        int i;
        for ( i = 0; i < count; i ++ ) {
                array = add_var ( array, 1 );
                array[i] = i;

                int *curr = get_var_last ( array );
                set_log_behavior ( LOG_OUTPUT_TO_FILE );
                log_normal ( "%d", array[i] );
        }
        log_normal ( "number element(added): %d", get_var_len ( array ) );
        for ( i = 0; i < count/2; i ++ ) {
                array = dec_var ( array, 2 );
        }
        log_normal ( "number element(deleted): %d", get_var_len ( array ) );
        log_normal ( "memory query 2: %dkb", query_mem_usage () );
        /* shrink-expand */
        array = shrink_var ( array );
        log_normal ( "number element(shrinked): %d", get_var_len ( array ) );
        log_normal ( "memory query 3: %dkb", query_mem_usage () );
        for ( i = 0; i < count; i ++ ) {
                array = expand_var ( array, i + 1 );
                array[i] = i;
        }
        log_normal ( "number element(expanded): %d", get_var_len ( array ) );
        flush_var ( array );
        log_normal ( "number element(flushed): %d", get_var_len ( array ) );
        free_var ( array );
        log_normal ( "memory query 4: %dkb", query_mem_usage () );
//	dbg_finish ();
}


// Test the error report code in X3dLogOutput
void ReportErrorTest ( void )
{
        init_log_output ( 0 );
        log_normal ( "This is a normal message" );
        log_mild_err ( "This is a mild error message" );
        log_severe_err ( "This is a severe error message" );
        log_critical_err ( "This is a critical error message" );
        int para = 10, para1 = 15;
        set_log_behavior ( LOG_OUTPUT_TO_FILE );
        log_normal ( "There are two numbers: %d %d", para, para1 );
//	free_log_output ();

}// End Function ReportErrorTest


// #include <algorithm>

#define PERFORMANCE_TEST
// #define CORRECTNESS_TEST
// #define COMPARISON_TEST
int CompareValue ( int *v0, int *v1, void *p );

int CompareValue ( int *v0, int *v1, void *p )
{
        return *v0 < *v1;
}

/*
int cmp ( const int &v0, const int &v1 );

int cmp ( const int &v0, const int &v1 )
{
	return v0 < v1;
}*/
// Test the SortArrayListQuick function in X3dAlgorithm
void SortArrayQuickTest ( void )
{
#ifdef PERFORMANCE_TEST	// Performance test
        const int n = 10000000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int begin = clock ();
        int k;
        for ( k = 0; k < 5; k ++ ) {
                int i;
                for ( i = 0; i < n; i ++ ) {
                        list[i] = rand ();
                }
                SortArrayQuick ( list, n, int, nullptr, CompareValue );
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        free_fix ( list );
#endif // PERFORMANCE_TEST

#ifdef CORRECTNESS_TEST	// Correctness test
        const int n = 100;
        int list[n];

        for ( int i = 0; i < n; i ++ ) {
                list[i] = rand ()%n + 1;
                log_normal ( "%d", list[i] );
        }

        SortArrayQuick ( list, n, int, nullptr, CompareValue );
        puts ( "\n" );
        for ( int i = 0; i < n - 1; i ++ ) {
                if ( list[i + 1] < list[i] ) {
                        log_normal ( "Failed" );
                }
        }
#endif	// CORRECTNESS_TEST

#ifdef COMPARISON_TEST	// Speed comparison test
        const int n = 10000000;
        int *list = new int[n];
        int begin = clock ();
        for ( int k = 0; k < 5; k ++ ) {
                for ( int i = 0; i < n; i ++ ) {
                        //		list[i] = rand ()%n + 1;
                        list[i] = rand ();
                }
                std::sort ( list, list + n, cmp );
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        delete list;
#endif	// COMPARISON_TEST

}// End Function SortArrayListQuickTest
#undef PERFORMANCE_TEST
#undef CORRECTNESS_TEST
#undef COMPARISON_TEST



// #define PERFORMANCE_TEST
#define CORRECTNESS_TEST
// #define COMPARISON_TEST
// Test the PartitionArrayList function in X3dAlgorithm
int Comparator ( int *listElement, int *midValue );
int Comparator ( int *listElement, int *midValue )
{
        return *listElement < *midValue;
}

int xmidValue = 0;
/*int predicate ( const int &n );
int predicate ( const int &n )
{
	return n < xmidValue;
}*/

void PartitionArrayTest ( void )
{
#ifdef PERFORMANCE_TEST	// Performance test
        const int n = 10000000;
        int *list = new int[n];
        int begin = clock ();
        for ( int k = 0; k < 20; k ++ ) {
                for ( int i = 0; i < n; i ++ ) {
                        list[i] = rand ()%n + 1;
                }
                int midIndex;
                int midValue = n >> 1;
                PartitionArrayMidValue ( list, n, int, midValue, nullptr, Comparator, midIndex );
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        delete list;
#endif	// PERFORMANCE_TEST

#ifdef CORRECTNESS_TEST	// Correctness test
        const int n = 100;
        int *list = alloc_fix ( sizeof ( int ), n );
        srand ( time ( nullptr ) );
        int i;
        for ( i = 0; i < n; i ++ ) {
                list[i] = rand ()%n + 1;
        }
        int midValue = n >> 1;
        int midIndex;
        PartitionArrayMidValue ( list, n, int, &midValue, Comparator, midIndex );
        for ( i = 0; i < n; i ++ ) {
                log_normal ( "%d: %d", i, list[i] );
        }
        log_normal ( "midValue: %d", midValue );
        log_normal ( "midIndex: %d, %d", midIndex, list[midIndex] );

        int failed = 0;
        for ( i = 0; i < midIndex; i ++ ) {
                if ( list[i] >= midValue ) {
                        log_normal ( "Lower half FAILED !" );
                        log_normal ( "%d %d", i, list[i] );
                        failed = 1;
                }
        }
        for ( i = midIndex; i < n; i ++ ) {
                if ( list[i] < midValue ) {
                        log_normal ( "Greater half FAILED !" );
                        log_normal ( "%d: %d", i, list[i] );
                        failed = 1;
                }
        }
        if ( !failed ) {
                log_normal ( "ALL CORRECT" );
        }
        free_fix ( list );
#endif	// CORRECTNESS_TEST

#ifdef COMPARISON_TEST	// Speed comparison test
        const int n = 10000000;
        int *list = new int[n];
        xmidValue = n >> 1;
        int begin = clock ();
        for ( int k = 0; k < 20; k ++ ) {
                for ( int i = 0; i < n; i ++ ) {
                        list[i] = rand ()%n + 1;
                }
                std::partition ( list, list + n, predicate );
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        delete list;
#endif	// COMPARISON_TEST

}// End Function PartitionArrayListTest
#undef PERFORMANCE_TEST
#undef CORRECTNESS_TEST
#undef COMPARISON_TEST



// #define PERFORMANCE_TEST
#define CORRECTNESS_TEST
// #define COMPARISON_TEST
// Test the SortListQuick function in X3dAlgorithm
void SortListQuickTest ( void )
{
#ifdef CORRECTNESS_TEST
        int biindexListTest = 1;
        int failed = 0;
        const int n = 1000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int i;
        for ( i = 0; i < n; i ++ ) {
                list[i] = rand ()%n + 1;
                log_normal ( "%d", list[i] );
        }

        if ( biindexListTest ) {
                ALG_BIINDEX_LIST_PTR orderList;
                CreateBiindexList ( &orderList, n );
                SortIndexQuick ( list, orderList, n, int, nullptr, CompareValue );
                ProcessBiindexList ( list, orderList, n, int );

                puts ( "\n" );
                for ( i = 0; i < n - 1; i ++ ) {
                        log_normal ( "%d", list[i] );
                        if ( list[i + 1] < list [i] ) {
                                failed = 1;
                        }
                }
                free_fix ( list );
        } else {
                int *newList = alloc_fix ( sizeof ( int ), n );
                ALG_INDEX_LIST_PTR orderList;
                CreateIndexList ( &orderList, n );
                SortIndexQuick ( list, orderList, n, int, nullptr, CompareValue );
                ProcessIndexListToArray ( list, orderList, n, int, newList );

                puts ( "\n" );
                for ( i = 0; i < n - 1; i ++ ) {
                        log_normal ( "%d", newList[i] );
                        if ( newList[i + 1] < newList [i] ) {
                                failed = 1;
                        }
                }
                free_fix ( newList );
                free_fix ( list );
        }
        if ( failed ) {
                log_normal ( "FAILED" );
        }
#endif // CORRECTNESS_TEST

}// End Function SortListQuickTest
#undef PERFORMANCE_TEST
#undef CORRECTNESS_TEST
#undef COMPARISON_TEST



// #define PERFORMANCE_TEST
// #define CORRECTNESS_TEST
// #define COMPARISON_TEST
// Test the FindNthElementArray function in X3dAlgorithm
#define cmp_find_nth_arr( a, b, p )	(*(a) < *(b))


void FindNthElementArrayTest ( void )
{
#ifdef PERFORMANCE_TEST
        const int n = 10000000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int begin = clock ();
        for ( int k = 0; k < 20; k ++ ) {
                for ( int index = 0; index < n; index ++ ) {
                        list[index] = rand ()%n + 1;
                }
                FindNthElementArray ( list, n, n >> 1, int, nullptr, cmp_find_nth_arr );
        }
        int end = clock ();
        log_normal ( "Nth is: %d, %d\n", n >> 1, list[n >> 1] );
        log_normal ( "Time: %f\n", (float) (end - begin)/CLOCKS_PER_SEC );
        free_fix ( list );
#endif // PERFORMANCE_TEST

#ifdef CORRECTNESS_TEST
        const int n = 1000;
        int *list = alloc_fix ( sizeof ( int ), n );
        srand ( time ( nullptr ) );
        int index;
        for ( index = 0; index < n; index ++ ) {
                list[index] = rand ()%n + 1;
        }
        FindNthElementArray ( list, n, 1000, int, nullptr, cmp_find_nth_arr );
        log_normal ( "Nth is: %d, %d\n", 1000, list[n >> 1] );
        std::nth_element ( list, list + 1000, list + n );
        log_normal ( "Nth is: %d, %d\n", 1000, list[n >> 1] );
        free_fix ( list );
#endif

#ifdef COMPARISON_TEST
        const int n = 10000000;
        int *list = alloc_fix ( sizeof ( int ), n );
        int begin = clock ();
        for ( int k = 0; k < 20; k ++ ) {
                for ( int index = 0; index < n; index ++ ) {
                        list[index] = rand ()%n + 1;
                }
                std::nth_element ( list, list + (n >> 1), list + n );
        }
        int end = clock ();
        log_normal ( "Nth is: %d, %d\n", n >> 1, list[n >> 1] );
        log_normal ( "Time: %f\n", (float) (end - begin)/CLOCKS_PER_SEC );
        free_fix ( list );
#endif // COMPARISON_TEST
}
#undef PERFORMANCE_TEST
#undef CORRECTNESS_TEST
#undef COMPARISON_TEST



// #define PERFORMANCE_TEST
// #define CORRECTNESS_TEST
// #define COMPARISON_TEST
// Test the FindNthElementIndex function in X3dAlgorithm

void FindNthElementIndexTest ( void )
{
#ifdef CORRECTNESS_TEST
        const int n = 1000;
        int *list = alloc_fix ( sizeof ( int ), n );
        srand ( time ( nullptr ) );
        struct ALG_BIINDEX_LIST_TYP *biindexList;
        CreateBiindexList ( &biindexList, n );
        int index;
        for ( index = 0; index < n; index ++ ) {
                list[index] = rand ()%n + 1;
        }
        FindNthElementIndex ( list, biindexList, n, n >> 1, int, nullptr, Compare );
        ProcessBiindexList ( list, biindexList, n, int );
        log_normal ( "Nth is: %d, %d\n", n >> 1, list[n >> 1] );
//	std::nth_element ( list, list + (n >> 1), list + n );
        log_normal ( "Nth is: %d, %d\n", n >> 1, list[n >> 1] );
        free_fix ( list );
#endif
}
#undef PERFORMANCE_TEST
#undef CORRECTNESS_TEST
#undef COMPARISON_TEST



#define PERFORMANCE_TEST
// #define CORRECTNESS_TEST
// #define COMPARISON_TEST
// Test the FindAddElementHashStatic in X3dAlgorithm
struct INFO_PACK {
        int *array;
        int currentData;
};

#define cmp_find_nth( _i, _info )	((_info)->array[_i] == (_info)->currentData)

void HashLookupTest ( void )
{
#ifdef CORRECTNESS_TEST
        int array[11] = {0, 4, 2, 3, 4, 5, 10, 2, 0, 3, 5};
        struct INFO_PACK infoPack;
        infoPack.array = array;
        struct alg_hash_link_list hashTable;
        InitHashTable ( 1 << 8, 0, &hashTable );
        for ( int i = 0; i < 11; i ++ ) {
                puts ( "Looking" );
                infoPack.currentData = array[i];
                int iFound;
                FindInsertHash ( &hashTable, i, array[i], iFound, &infoPack, cmp_find_nth, DynamicHash );
                if ( iFound != ALG_HASH_INVALID ) {
                        log_normal ( "index(%d), data(%d)\nindex(%d), data(%d)\n",
                                     iFound, array[iFound], i, array[i] );
                }
        }
        ReleaseHashTable ( &hashTable );
#endif // CORRECTNESS_TEST

#ifdef PERFORMANCE_TEST
        const int n = 20000;
        int array[n];
        int i;
        for ( i = 0; i < n; i ++ ) {
                array[i] = rand ()%n;
        }
        struct INFO_PACK infoPack;
        infoPack.array = array;
        struct alg_hash_link_list hashTable;
        InitHashTable ( 1 << 15, 1, &hashTable );
        int begin = clock ();
        int k;
        for ( k = 0; k < 10000; k ++ ) {
                for ( i = 0; i < n; i ++ ) {
                        infoPack.currentData = array[i];
                        int iFound;
                        FindInsertHash ( &hashTable, i, array[i], iFound, &infoPack, cmp_find_nth, DynamicHash );
                }
        }
        int end = clock ();
        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
        ReleaseHashTable ( &hashTable );
#endif // PERFORMANCE_TEST
}// End Function HashLookupStaticTest
#undef PERFORMANCE_TEST
#undef CORRECTNESS_TEST
#undef COMPARISON_TEST
