#ifndef CONTAINER_H_INCLUDED
#define CONTAINER_H_INCLUDED


#include <x3d/common.h>
#include <system/allocator.h>


#define alg_array(type, _self)                                alg_##type##_array(_self)
#define alg_iter(_type)                                       _type*
#define alg_access(_iter)                                     (*(_iter))
#define alg_n(type, _self)                                    alg_##type##_n(_self)
#define alg_init(type, _self, _elm_size, _init_count)         alg_##type##_init(_self, _elm_size, _init_count)
#define alg_free(type, _self)                                 alg_##type##_free(_self)
#define alg_push_back(type, _self, _data)                     alg_##type##_push_back(_self, _data)
#define alg_push_front(type, _self, _data)                    alg_##type##_push_front(_self, _data)
#define alg_pop_back(type, _self, _iter)                      alg_##type##_pop_back(_self, _iter)
#define alg_pop_front(type, _self, _iter)                     alg_##type##_pop_front(_self, _iter)
#define alg_first(type, _self, _iter)                         alg_##type##_first(_self, _iter)
#define alg_next(type, _self, _iter)                          alg_##type##_next(_self, _iter)
#define alg_prev(type, _self, _iter)                          alg_##type##_prev(_self, _iter)
#define alg_last(type, _self, _iter)                          alg_##type##_last(_self, _iter)
#define alg_null(type, _self, _iter)                          alg_##type##_null(_self, _iter)
#define alg_find(type, _self, _data, _iter, _cmp)             alg_##type##_find(_self, _data, _iter, _cmp)
#define alg_insert(type, _self, _data, _iter, _cmp)           alg_##type##_insert(_self, _data, _iter, _cmp)
#define alg_inject(type, _self, _data, _iter)                 alg_##type##_inject(_self, _data, _iter)
#define alg_remove(type, _self, _iter)                        alg_##type##_remove(_self, _iter)
#define alg_flush(type, _self)                                alg_##type##_flush(_self)
#define alg_expand(type, _self, _n)                           alg_##type##_expand(_self, _n);


#define d_alg_list(_type)               struct alg_list
#define d_alg_llist(_type)              struct alg_llist
#define d_alg_btree(_type)              struct alg_btree
#define d_alg_hash_llist(_type)         struct alg_hash_llist


#define NIL_POSITION		-1

#endif // CONTAINER_H_INCLUDED
