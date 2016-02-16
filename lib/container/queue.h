#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <system/allocator.h>


/*
 * <queue> decl
 */
#define queue_templ(T)                  \
struct {                                \
        T*              values;        \
        int             num_elm;        \
}

/*
 * <queue> public
 */
#define queue_init(_self, _init_count)                                                  \
{                                                                                       \
        zero_obj(_self);                                                                \
        if (_init_count < 0) {                                                          \
                (_self)->values = alloc_var(sizeof(*(_self)->values), 0);               \
        } else {                                                                        \
                (_self)->values = alloc_var(sizeof(*(_self)->values), _init_count);     \
        }                                                                               \
        (_self)->num_elm   = 0;                                                         \
}

#define queue_free(_self)               \
{                                       \
        free_var((_self)->values);      \
        zero_obj(_self);                \
}

#define queue_expand(_self, _count)                                     \
{                                                                       \
        (_self)->values = alloc_var_expand2((_self)->values, count);    \
}

#define queue_flush(_self)                      \
{                                               \
        alloc_var_flush((_self)->values);       \
        (_self)->num_elm = 0;                   \
}

/* Both lists must have the same element type */
#define queue_copy(_self, _to_copy)                                                             \
{                                                                                               \
        alloc_var_expand((_self)->values, (_to_copy)->num_elm);                                 \
        memcpy((_self)->values, _to_copy, (_to_copy)->num_elm*sizeof(*(_self)->values));        \
        (_self)->num_elm = (_to_copy)->num_elm;                                                 \
}

/* swaps the self's pointer respectively */
#define queue_swap(_self, _to_swap)                     \
{                                                       \
        void* p                 = (_self)->values;      \
        (_self)->values        = (_to_swap)->values;    \
        (_to_swap)->values     = p;                     \
        int t                   = (_self)->num_elm;     \
        (_self)->num_elm        = (_to_swap)->num_elm;  \
        (_to_swap)->num_elm     = t;                    \
}

#define queue_push_back(_self, _data)                                                   \
{                                                                                       \
        (_self)->values                        = alloc_var_add((_self)->values, 1);     \
        (_self)->values[(_self)->num_elm]      = (_data);                               \
        (_self)->num_elm ++;                                                            \
}

#define queue_pop_back(_self)                                   \
{                                                               \
        (_self)->values = alloc_var_dec((_self)->values, 1);    \
        (_self)->num_elm --;                                    \
}

#define queue_peek(_self, _data)                                \
{                                                               \
        (_data) = (_self)->values[(_self)->num_elm - 1];        \
}

#define queue_size(_self)                ((_self)->num_elm)

#define queue_to_array(_self)            ((_self)->values)

/*
 * <queue_iterator> decl
 */
#define queue_iter_templ(T)     \
struct {                        \
        T*      curr_pos;       \
        T*      last;           \
}
/*
 * <queue_iterator> public
 */
#define queue_iter_begin(_self, _list)                          \
{                                                               \
        (_self)->curr_pos = (_list)->values;                    \
        (_self)->last = &(_list)->values[(_list)->num_elm];     \
}

#define queue_iter_next(_self, _list)                           \
{                                                               \
        (_self)->curr_pos ++;                                   \
}

#define queue_iter_has_value(_self, _list)                      \
        ((_self)->curr_pos != (_self)->last)

#define queue_iter_deref(_self, _list)                          \
        (*(_self)->curr_pos)

#define queue_for_each(_self, _elm, CODE)                       \
{                                                               \
        queue_iter_templ(typeof(*(_self)->values)) _iterator;   \
        queue_iter_begin(&_iterator, _self);                    \
        while (queue_iter_has_value(&_iterator, _self)) {       \
                (_elm) = queue_iter_deref(&_iterator, _self);   \
                CODE;                                           \
                queue_iter_next(&_iterator, _self);             \
        }                                                       \
}


#endif	// LIST_H_INCLUDED
