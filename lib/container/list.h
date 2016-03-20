#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED

#include <system/allocator.h>

#define NIL             -1

/*
 * <list_iterator> decl
 */
#define list_iter_templ(T)                                                                \
struct {                                                                                  \
        T*      type;                                                                     \
        int     pos;                                                                      \
}

/*
 * <list_iterator> public
 */
#define list_iter_begin(_self, _list)                                                     \
{                                                                                         \
        (_self)->pos = (_list)->nodes[0].next;                                            \
}

#define list_iter_next(_self, _list)                                                      \
{                                                                                         \
        (_self)->pos = (_list)->nodes[(_self)->pos].next;                                 \
}

#define list_iter_prev(_self, _list)                                                      \
{                                                                                         \
        (_self)->pos = (_list)->nodes[(_self)->pos].prev;                                 \
}

#define list_iter_has_next(_self, _list)                                                  \
        ((_list)->nodes[(_self)->pos].next != NIL)

#define list_iter_has_prev(_self, _list)                                                  \
        ((_list)->nodes[(_self)->pos].prev != NIL)

#define list_iter_deref(_self, _list)                                                     \
        ((_list)->nodes[(_self)->pos].value)

#define list_for_each(_self, _elm, CODE)                                                  \
{                                                                                         \
        list_iter_templ(typeof(*(_self)->content)) _iterator;                             \
        list_iter_begin(&_iterator, _self);                                               \
        while (list_iter_has_next(&_iterator, _self)) {                                   \
                (_elm) = list_iter_deref(&_iterator, _self);                              \
                CODE;                                                                     \
                list_iter_next(&_iterator, _self);                                        \
        }                                                                                 \
}

/*
 * <list> decl
 * This list data type use header&trailer linked list as its implementation.
 * Header node is the zeroth element nodes[0], and trailer node is element nodes[1].
 */
#define list_templ(T)                                                                     \
struct {                                                                                  \
        struct {                                                                          \
                T       value;                                                            \
                int     next;                                                             \
                int     prev;                                                             \
        }      *nodes;                                                                    \
        int num_elm;                                                                      \
        int recycle_head;                                                                 \
        int total_size;                                                                   \
}

/*
 * <list> public
 */
#define list_init(_self, _init_count)                                                     \
{                                                                                         \
        (_self)->nodes          = alloc_var(sizeof(*(_self)->nodes), 2 + (_init_count));  \
        (_self)->nodes[0].prev  = NIL;                                                    \
        (_self)->nodes[0].next  = 1;                                                      \
        (_self)->nodes[1].prev  = 0;                                                      \
        (_self)->nodes[1].next  = NIL;                                                    \
        (_self)->num_elm        = 0;                                                      \
        (_self)->recycle_head   = NIL;                                                    \
        (_self)->total_size     = 2;                                                      \
}

#define list_free(_self)                                                                  \
{                                                                                         \
        free_var((_self)->nodes);                                                         \
        zero_obj((_self));                                                                \
}

#define list_flush(_self)                                                                 \
{                                                                                         \
        alloc_var_flush((_self)->nodes);                                                  \
        (_self)->nodes[0].next  = 1;                                                      \
        (_self)->nodes[1].prev  = 0;                                                      \
        (_self)->num_elm        = 0;                                                      \
        (_self)->recycle_head       = NIL;                                                \
        (_self)->total_size     = 2;                                                      \
}

#define list_to_array(_self, _array)                                                      \
{                                                                                         \
        int _i;                                                                           \
        for (_i = 0; _i < (_self)->num_elm: _i ++) {                                      \
                _array[_i] = (_self)->nodes[_i].value;                                    \
        }                                                                                 \
}

#define __list_alloc_new_node(_self, _pos)                                                \
{                                                                                         \
        if ((_self)->recycle_head == NIL) {                                               \
                (_pos) = (_self)->total_size;                                             \
                (_self)->nodes = alloc_var_expand2((_self)->nodes, ++ (_self)->total_size);                \
        } else {                                                                          \
                (_pos) = (_self)->recycle_head;                                           \
                (_self)->recycle_head = (_self)->nodes[(_self)->recycle_head].next;       \
        }                                                                                 \
}

#define __list_free_node(_self, _pos)                                                     \
{                                                                                         \
        if ((_self)->recycle_head == NIL) {                                               \
                (_self)->nodes[_pos].next = NIL;                                          \
        } else {                                                                          \
                (_self)->nodes[_pos].next = (_self)->recycle_head;                        \
        }                                                                                 \
        (_self)->recycle_head = _pos;                                                     \
}

#define __list_insert_before(_self, _data, _i)                                            \
{                                                                                         \
        int _new_pos;                                                                     \
        __list_alloc_new_node(_self, _new_pos);                                           \
        int _old_prev = (_self)->nodes[_i].prev;                                          \
                                                                                          \
        (_self)->nodes[_new_pos].value = (_data);                                         \
        (_self)->nodes[_new_pos].next = (_i);                                             \
        (_self)->nodes[_new_pos].prev = _old_prev;                                        \
                                                                                          \
        (_self)->nodes[_i].prev = _new_pos;                                               \
        (_self)->nodes[_old_prev].next  = _new_pos;                                       \
        (_self)->num_elm ++;                                                              \
}

// insert before the iterator
#define list_insert(_self, _data, _iter)                                                  \
{                                                                                         \
        __list_insert_before(_self, _data, (_iter)->pos);                                 \
}

#define list_get_by_value(_self, _data, f_Is_Equal, _iter)                                \
{                                                                                         \
        while ((_self)->nodes[(_iter)->pos].next != NIL) {                                \
                if (f_Is_Equal(&_data, &(_self)->nodes[(_iter)->pos].value))                \
                        break;                                                            \
                (_iter)->pos = (_self)->nodes[(_iter)->pos].next;                          \
        }                                                                                 \
}

#define list_push_back(_self, _data)                                                      \
{                                                                                         \
        __list_insert_before(_self, _data, 1);                                            \
}

#define list_push_front(_self, _data)                                                     \
{                                                                                         \
        __list_insert_before(_self, _data, (_self)->nodes[0].next);                       \
}

#define __list_remove_at_i(_self, _i, _prev, _next)                                       \
{                                                                                         \
        _prev = (_self)->nodes[_i].prev;                                                  \
        _next = (_self)->nodes[_i].next;                                                  \
                                                                                          \
        (_self)->nodes[_prev].next = _next;                                               \
        (_self)->nodes[_next].prev = _prev;                                               \
                                                                                          \
        __list_free_node(_self, _i);                                                      \
        (_self)->num_elm --;                                                              \
}

#define list_remove_at(_self, _iter)                                                      \
{                                                                                         \
        int _prev, _next;                                                                 \
        __list_remove_at_i(_self, (_iter)->pos, _prev, _next);                            \
        (_iter)->pos = _prev;                                                             \
}

#define list_front(_self, _value)                                                         \
{                                                                                         \
        (_value) = (_self)->nodes[(_self)->nodes[0].next].value;                          \
}

#define list_back(_self, _value)                                                          \
{                                                                                         \
        (_value) = (_self)->nodes[(_self)->nodes[1].prev].value;                          \
}

#define list_pop_front(_self)                                                             \
{                                                                                         \
        if ((_self)->num_elm > 0) {                                                       \
                int _prev, _next;                                                         \
                __list_remove_at_i(_self, (_self)->nodes[0].next, _prev, _next);          \
                (_self)->nodes[0].next = _next;                                           \
        }                                                                                 \
}

#define list_pop_back(_self)                                                              \
{                                                                                         \
        if ((_self)->num_elm > 0) {                                                       \
                int _prev, _next;                                                         \
                __list_remove_at_i(_self, (_self)->nodes[1].prev);                        \
                (_self)->nodes[1].prev = _prev;                                           \
        }                                                                                 \
}

#define list_size(_self)              ((_self)->num_elm)


#endif	// DEQUE_H_INCLUDED
