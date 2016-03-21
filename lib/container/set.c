#include <container/set.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>
//
//
//static void set_correctness_test( struct alg_named_params *param )
//{
//        int array[11] = {0, 4, 2, 3, 4, 5, 10, 2, 0, 3, 5};
//
//        struct alg_hash_llist hash_llist;
//        alg_hash_llist_init(&hash_llist, 1 << 8, 0, sizeof (int));
//        int i;
//        for ( i = 0; i < 11; i ++ ) {
//                log_normal ( "Looking up..." );
//                int *rfound = nullptr;
//                alg_hash_llist_first ( &hash_llist, i, &rfound, &array[i], hash_cmp_val );
//
//                if ( rfound != nullptr ) {
//                        log_normal ( "index(%d), data(%d)\nindex(%d), data(%d)\n",
//                                     rfound - array, *rfound, i, array[i] );
//                } else {
//                        log_normal ( "couldn't find such element: index(%d), data(%d). it is now inserted.", i, array[i] );
//                        alg_hash_llist_add ( &hash_llist, &array[i], i, f_Dym_Update );
//                }
//        }
//        alg_hash_llist_free(&hash_llist);
//
//}
//
//static void hash_container_p ( struct alg_named_params *param )
//{
//        const int n = 20000;
//        int array[n];
//        int i;
//        for ( i = 0; i < n; i ++ ) {
//                array[i] = rand ()%n;
//        }
//
//        struct alg_hash_llist hash_llist;
//        alg_hash_llist_init(&hash_llist, 1 << 15, 1, sizeof (int));
//        int begin = clock ();
//        int k;
//        for ( k = 0; k < 10000; k ++ ) {
//                for ( i = 0; i < n; i ++ ) {
//                        int *rfound = nullptr;
//                        alg_hash_llist_first (
//                                &hash_llist, gen_hash32 (array[i]), &rfound,
//                                &array[i], hash_cmp_val );
//
//                        if ( rfound == nullptr ) {
//                                alg_hash_llist_add ( &hash_llist, &array[i],
//                                                     gen_hash32 (array[i]), f_Dym_Update );
//                        }
//                }
//        }
//        int end = clock ();
//        log_normal ( "%f", (float) (end - begin)/CLOCKS_PER_SEC );
//        alg_hash_llist_free(&hash_llist);
//}
