#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED

#include <system/allocator.h>

#define NIL             -1

/*
 * <list_iterator> decl
 */
#define list_iter_templ(T)     \
struct {                        \
        T*      type;           \
        int     pos;            \
}

/*
 * <list_iterator> public
 */
#define list_iter_begin(_self, _list)         \
{                                               \
        (_self)->pos = (_list)->head[0];       \
}

#define list_iter_next(_self, _list)                          \
{                                                               \
        (_self)->pos    = (_list)->next[(_self)->pos];         \
}

#define list_iter_prev(_self, _iter)                           \
{                                                               \
        (_self)->pos    = (_list)->prev[(_self)->pos];         \
}

#define list_iter_has_value(_self, _list)                      \
        ((_list)->next[(_self)->pos] != (_list)->icurr)

#define list_iter_deref(_self, _list)                         \
        ((_list)->content[(_self)->pos])

#define list_for_each(_self, _elm, CODE)                       \
{                                                               \
        list_iter_templ(typeof(*(_self)->content)) _iterator;  \
        list_iter_begin(&_iterator, _self);                    \
        while (list_iter_has_value(&_iterator, _self)) {        \
                (_elm) = list_iter_deref(&_iterator, _self);   \
                CODE;                                           \
                list_iter_next(&_iterator, _self);             \
        }                                                       \
}

/*
 * <list> decl
 * This list data type use header linked list as its implementation.
 * Header node is the zeroth element.
 */
#define list_templ(T)           \
struct {                        \
        struct {                \
                T       value;  \
                int     next;   \
        }      *nodes;          \
        int     trailer;        \
        int     num_mem;        \
        int     last_mem:       \
        int     num_elm;        \
}

/*
 * <list> public
 */
#define list_init(_self, _init_count)                                                   \
{                                                                                       \
        (_self)->nodes          = alloc_var(sizeof(*(_self)->nodes), 1 + (_init_count));\
        (_self)->node[0].next   = NIL;                                                  \
        (_self)->trailer        = 0;                                                    \
        (_self)->num_mem        = 1;                                                    \
        (_self)->last_mem       = 0;                                                    \
        (_self)->num_elm        = 0;                                                    \
}

#define list_free(_self)                \
{                                       \
        free_var((_self)->node);        \
        zero_obj((_self));              \
}

#define list_flush(_self)                               \
{                                                       \
        alloc_var_flush((_self)->nodes);                \
        (_self)->node[0].next   = NIL;                  \
        (_self)->num_mem      = 1;                      \
        (_self)->last_mem      = 1;                     \
        (_self)->num_elm        = 0;                    \
}

#define list_to_array(_self, _array)                    \
{                                                       \
        int _i;                                         \
        for (_i = 0; _i < (_self)->num_elm: _i ++) {    \
                _array[_i] = (_self)->nodes[_i].value;  \
        }                                               \
}

#define __list_alloc_new_node(_self, _pos)                                                      \
{                                                                                               \
        if ((_self)->last_mem == (_self)->num_mem) {                                            \
                /* running out of memory, replenish by acquiring a larger array */              \
                (_self)->nodes = alloc_var_expand2((_self)->nodes, ++ (_self)->num_mem);        \
                (_pos) = (_self)->last_mem ++;                                                  \
        } else {                                                                                \
                /* still have enough memory to work on */                                       \
                (_pos) = (_self)->last_mem;                                                     \
                (_self)->last_mem = (_self)->nodes[(_self)->last_mem];                          \
        }                                                                                       \
}

#define __list_free_node(_self, _pos)                                           \
{                                                                               \
        // link to the back of the trailer and point it to the last_mem         \
        // move the last_mem to here                                            \
        (_self)->nodes[(_self)->trailer].next = _pos;                           \
        (_self)->nodes[_pos].next = (_self)->last_mem;                          \
        (_self)->last_mem = _pos;                                               \
}

#define list_insert(_self, _data, _iter)                        \
{                                                               \
        /* move current node to new node */                     \
        int _new_pos;                                           \
        __list_alloc_new_node(_self, _new_pos);                 \
        (_self)->nodes[_new_pos] = (_self)->nodes[(_iter)->pos];\
        (_self)->nodes[(_iter)->pos].value = (_data);           \
        (_self)->nodes[(_iter)->pos].next  = _new_pos;          \
        (_self)->num_elm ++;                                    \
}

#define list_get_by_value(_self, _data, f_Is_Equal, _iter) \
{ \
}

#define list_push_back(_self, _data)                                                            \
{                                                                                               \
}

#define list_push_front(_self, _data)                                                           \
{                                                                                               \
        (_self)->prev           = alloc_var_expand2((_self)->prev, (_self)->num_elm);           \
        (_self)->head           = (_self)->prev;                                                \
        (_self)->next           = alloc_var_expand2((_self)->next, (_self)->num_elm);           \
        (_self)->content        = alloc_var_expand2((_self)->content, (_self)->num_elm);        \
                                                                                                \
        int _i = (_self)->icurr;                                                                \
        (_self)->content[_i]    = (_data);                                                      \
                                                                                                \
        if ((_self)->icurr == (_self)->ilast) {                                                 \
                (_self)->icurr                = ++ (_self)->ilast;                              \
                (_self)->prev[(_self)->icurr] = (_self)->prev[_i];                              \
        } else                                                                                  \
                (_self)->icurr                = (_self)->next[_i];                              \
                                                                                                \
        (_self)->next[(_self)->prev[_i]] = (_self)->icurr;                                      \
        (_self)->prev[(_self)->head[0]]  = _i;                                                  \
        (_self)->prev[_i]                = NIL_POSITION;                                        \
        (_self)->next[_i]                = (_self)->head[0];                                    \
        (_self)->head[0]                 = _i;                                                  \
        (_self)->num_elm ++;                                                                    \
}

#define __list_remove_at_i(_self, _i)                                                           \
{                                                                                               \
        (_self)->prev[(_self)->next[(_i)]]                   = (_self)->prev[(_i)];             \
        (_self)->next[(_self)->prev[(_i)]]                   = (_self)->next[(_i)];             \
        if ((_self)->icurr != (_self)->ilast)                                                   \
                (_self)->prev[(_self)->next[(_self)->icurr]] = (_i);                            \
        (_self)->next[(_self)->icurr]                        = (_i);                            \
                                                                                                \
        (_self)->num_elm --;                                                                    \
}

#define list_remove_at(_self, _iter)                                                           \
{                                                                                               \
        __list_remove_at_i(_self, (_iter)->pos);                                               \
}

#define list_front(_self, _value)                       \
{                                                       \
        (_value) = (_self)->content[(_self)->head[0]];  \
}

#define list_back(_self, _value)                        \
{                                                       \
        (_value) = (_self)->content[(_self)->ilast];    \
}

#define list_pop_front(_self)                                   \
{                                                               \
        if ((_self)->icurr != (_self)->head[0]) {               \
                __list_remove_at_i(_self, (_self)->head[0]);    \
        }                                                       \
}

#define list_pop_back(_self)                                                    \
{                                                                               \
        if ((_self)->icurr != (_self)->head[0]) {                               \
                __list_remove_at_i(_self, (_self)->prev[(_self)->icurr]);       \
        }                                                                       \
}

#define list_size(_self)              ((_self)->num_elm)
             

#endif	// DEQUE_H_INCLUDED
