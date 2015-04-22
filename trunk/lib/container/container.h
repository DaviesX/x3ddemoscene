#ifndef CONTAINER_H_INCLUDED
#define CONTAINER_H_INCLUDED


#include <x3d/common.h>
#include <allocator.h>


#define alg_array( type, _inst )                                alg_##type##_array ( _inst )
#define alg_iter( _type )                                       _type*
#define alg_access( _iter )                                     (*(_iter))
#define alg_n( type, _inst )                                    alg_##type##_n( _inst )
#define alg_init( type, _inst, _elm_size, _init_count )         create_alg_##type ( _inst, _elm_size, _init_count )
#define alg_free( type, _inst )                                 free_alg_##type ( _inst )
#define alg_push_back( type, _inst, _data )                     alg_##type##_push_back ( _data, _inst )
#define alg_push_front( type, _inst, _data )                    alg_##type##_push_front ( _data, _inst )
#define alg_pop_back( type, _inst, _iter )                      alg_##type##_pop_back ( _iter, _inst )
#define alg_pop_front( type, _inst, _iter )                     alg_##type##_pop_front ( _iter, _inst )
#define alg_first( type, _inst, _iter )                         alg_##type##_first ( _iter, _inst )
#define alg_next( type, _inst, _iter )                          alg_##type##_next ( _iter, _inst )
#define alg_prev( type, _inst, _iter )                          alg_##type##_prev ( _iter, _inst )
#define alg_last( type, _inst, _iter )                          alg_##type##_last ( _iter, _inst )
#define alg_null( type, _inst, _iter )                          alg_##type##_null ( _iter, _inst )
#define alg_find( type, _inst, _data, _iter, _cmp )             alg_##type##_find( _data, _iter, _cmp, _inst )
#define alg_insert( type, _inst, _data, _iter, _cmp )           alg_##type##_insert( _data, _iter, _cmp, _inst )
#define alg_inject( type, _inst, _data, _iter )                 alg_##type##_inject( _data, _iter, _inst )
#define alg_remove( type, _inst, _iter )                        alg_##type##_remove( _iter, _inst )
#define alg_flush( type, _inst )                                alg_##type##_flush( _inst )
#define alg_expand( type, _inst, _n )                           alg_##type##_expand( _n, _inst );


#define d_alg_list(_type)               struct alg_list
#define d_alg_llist(_type)              struct alg_llist
#define d_alg_btree(_type)              struct alg_btree
#define d_alg_hash_llist(_type)         struct alg_hash_llist


#endif // CONTAINER_H_INCLUDED
