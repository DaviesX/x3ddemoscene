#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED


/*
 * <deque> decl
 * bidirectional queue with header&tailer node, sort of like a light weight list that couldn't insert elements in the middle.
 */
#define deque_templ(T)                                                                                       \
struct {                                                                                                     \
        T*              values;                                                                              \
        int             front;                                                                               \
        int             rear;                                                                                \
        int             num_elm;                                                                             \
        int             mem_capacity;                                                                        \
}

/*
 * <deque> public
 */
#define deque_init(_self, _init_count)                                                                       \
{                                                                                                            \
        zero_obj(_self);                                                                                     \
        (_self)->front = 0;                                                                                  \
        (_self)->rear = 1;                                                                                   \
        (_self)->num_elm = 0;                                                                                \
        (_self)->mem_capacity = (_init_count) >= 2 ? (_init_count) : 2;                                      \
        (_self)->values = alloc_fix(sizeof(*(_self)->values), (_self)->mem_capacity);                        \
}

#define deque_free(_self)                                                                                    \
{                                                                                                            \
        free_fix((_self)->values);                                                                           \
        zero_obj(_self);                                                                                     \
}

#define deque_expand(_self, _count)                                                                          \
{                                                                                                            \
        __deque_ensure_capacity((_self)->values, _count);                                                          \
}

#define deque_flush(_self)                                                                                   \
{                                                                                                            \
        (_self)->front = 0;                                                                                  \
        (_self)->rear = 1;                                                                                   \
        (_self)->num_elm = 0;                                                                                \
}

/* Both lists must have the same element type */
#define deque_copy(_self, _to_copy)                                                                          \
{                                                                                                            \
        alloc_var_expand((_self)->values, (_to_copy)->num_elm);                                              \
        memcpy((_self)->values, _to_copy, (_to_copy)->num_elm*sizeof(*(_self)->values));                     \
        (_self)->num_elm = (_to_copy)->num_elm;                                                              \
        (_self)->front = (_to_copy)->front;                                                                  \
        (_self)->rear = (_to_copy)->rear;                                                                    \
}

#define __deque_swap(_x, _y)                                                                                       \
{                                                                                                            \
        typeof(_x) _t = (_x);                                                                                \
        (_x) = (_y);                                                                                         \
        (_y) = _t;                                                                                           \
}

/* swaps the self's pointer respectively */
#define deque_swap(_self, _to_swap)                                                                          \
{                                                                                                            \
        __deque_swap((_self)->values, (_to_swap)->values);                                                         \
        __deque_swap((_self)->num_elm, (_to_swap)->num_elm);                                                       \
        __deque_swap((_self)->rear, (_to_swap)->rear);                                                             \
        __deque_swap((_self)->front, (_to_swap)->front);                                                           \
}

#define __deque_max(a, b)  (((a) > (b)) ? (a) : (b))

#define __deque_pos(__i, __cap)    ((__i) >= (__cap) ? (__i) - (__cap) : (__i))

#define __deque_ensure_capacity(_self, _elm_count)                                                                 \
{                                                                                                            \
        if ((_elm_count) + 2 > (_self)->mem_capacity) {                                                      \
                int _new_cap = __deque_max((_self)->mem_capacity*2, _elm_count + 2);                               \
                typeof((_self)->values) _tmp = alloc_fix(sizeof(*(_self)->values), _new_cap);                \
                                                                                                             \
                int _i;                                                                                      \
                for (_i = 1; _i <= (_self)->num_elm; _i ++) {                                                \
                        _tmp[_i] = (_self)->values[__deque_pos((_self)->front + _i, (_self)->mem_capacity)];       \
                }                                                                                            \
                free_fix((_self)->values);                                                                   \
                                                                                                             \
                (_self)->mem_capacity = _new_cap;                                                            \
                (_self)->front = 0;                                                                          \
                (_self)->rear = (_self)->num_elm + 1;                                                        \
                (_self)->values = _tmp;                                                                      \
        }                                                                                                    \
}

#define deque_push_back(_self, _data)                                                                        \
{                                                                                                            \
        __deque_ensure_capacity(_self, (_self)->num_elm + 1);                                                      \
        (_self)->values[(_self)->rear] = (_data);                                                            \
        (_self)->rear = __deque_pos((_self)->rear + 1, (_self)->mem_capacity);                                     \
        (_self)->num_elm ++;                                                                                 \
}

#define deque_push_front(_self, _data)                                                                       \
{                                                                                                            \
        __deque_ensure_capacity(_self, (_self)->num_elm + 1);                                                      \
        (_self)->values[(_self)->front] = (_data);                                                           \
        (_self)->front = __deque_pos((_self)->front + (_self)->mem_capacity - 1, (_self)->mem_capacity);           \
        (_self)->num_elm ++;                                                                                 \
}

#define deque_pop_front(_self)                                                                               \
{                                                                                                            \
        if ((_self)->num_elm > 0) {                                                                          \
                (_self)->front = __deque_pos((_self)->front + 1, (_self)->mem_capacity);                           \
                (_self)->num_elm --;                                                                         \
        }                                                                                                    \
}

#define deque_pop_back(_self)                                                                                \
{                                                                                                            \
        if ((_self)->num_elm > 0) {                                                                          \
                (_self)->rear = __deque_pos((_self)->rear + (_self)->mem_capacity - 1, (_self)->mem_capacity);     \
                (_self)->num_elm --;                                                                         \
        }                                                                                                    \
}

#define deque_front(_self, _data)                                                                            \
{                                                                                                            \
        (_data) = (_self)->values[__deque_pos((_self)->front + 1, (_self)->mem_capacity)];                         \
}

#define deque_back(_self, _data)                                                                             \
{                                                                                                            \
        (_data) = (_self)->values[__deque_pos((_self)->rear + (_self)->mem_capacity - 1, (_self)->mem_capacity)];  \
}

#define deque_size(_self)                ((_self)->num_elm)

#define deque_to_array(_self)            ((_self)->values)

/*
 * <deque_iterator> decl
 */
#define deque_iter_templ(T)                                                                                  \
struct {                                                                                                     \
        int i;                                                                                               \
}
/*
 * <deque_iterator> public
 */
#define deque_iter_begin(_self, _deque)                                                                      \
{                                                                                                            \
        (_self)->i = 1;                                                                                      \
}

#define deque_iter_next(_self, _deque)                                                                       \
{                                                                                                            \
        (_self)->i ++;                                                                                       \
}

#define deque_iter_prev(_self, _deque)                                                                       \
{                                                                                                            \
        (_self)->i --;                                                                                       \
}

#define deque_iter_has_next(_self, _deque)                                                                   \
        ((_self)->i <= (_deque)->num_elm)

#define deque_iter_has_prev(_self, _deque)                                                                   \
        ((_self)->i > 0)

#define deque_iter_deref(_self, _deque)                                                                      \
        ((_deque)->values[__deque_pos((_deque)->front + (_self)->i, (_deque)->mem_capacity)])

#define deque_for_each(_self, _elm, CODE)                                                                    \
{                                                                                                            \
        deque_iter_templ(typeof(*(_self)->values)) _iterator;                                                \
        deque_iter_begin(&_iterator, _self);                                                                 \
        while (deque_iter_has_next(&_iterator, _self)) {                                                     \
                (_elm) = deque_iter_deref(&_iterator, _self);                                                \
                CODE;                                                                                        \
                deque_iter_next(&_iterator, _self);                                                          \
        }                                                                                                    \
}


#endif	// DEQUE_H_INCLUDED
