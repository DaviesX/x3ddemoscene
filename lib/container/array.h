#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include <system/allocator.h>

/*
 * <array> decl
 */
#define array_templ(T, K)                                                                                \
struct {                                                                                                 \
        T*      content;                                                                                 \
        T*      end;                                                                                     \
        K*      data;                                                                                    \
}
/*
 * <array> public
 */
#define array_init(_self, _begin, _end, _data) 	                                                         \
{                                                                                                        \
        (_self)->content = (_begin);		                                                         \
        (_self)->end     = (_end);		                                                         \
        (_self)->data    = (_data);                                                                      \
}

#define array_free(_self)		                                                                 \
{                                                                                                        \
        free_fix((_self)->content);	                                                                 \
        zero_obj(_self);		                                                                 \
}

#define array_copy(_self, _dest)                                                                         \
{                                                                                                        \
}

#define array_bipartition(_self, _pivot_value, f_Is_Less_Than, _bound)                                   \
{                                                                                                        \
        typeof((_self)->content) _ptr;                                                                   \
        typeof(_pivot_value) _pv = _pivot_value;                                                         \
        __array_bipartition(_self, (_self)->content, (_self)->end, _pv, f_Is_Less_Than, _ptr);           \
        *(_bound) = _ptr - (_self)->content;                                                             \
}

/*
 * <array> private
 */
#define __array_insert_sort(_self, _pstart, _pend, f_Is_Less_Than)                                       \
{                                                                                                        \
        int _size = (_pend) - (_pstart);                                                                 \
        int _i;                                                                                          \
        for (_i = 0; _i < _size; _i ++) {                                                                \
                if (f_Is_Less_Than(&(_pstart)[_i],                                                       \
                                   &(_pstart)[_i - 1],                                                   \
                                    (_self)->data)) {                                                    \
                        typeof (*_pstart) _ex = (_pstart)[_i];                                           \
                        int _j;                                                                          \
                        for (_j = _i; _j > 0; _j --) {                                                   \
                                if (f_Is_Less_Than(&_ex,                                                 \
                                                   &(_pstart)[_j - 1],                                   \
                                                    (_self)->data)) {                                    \
                                        (_pstart)[_j] = (_pstart)[_j - 1];                               \
                                } else {                                                                 \
                                        break;                                                           \
                                }                                                                        \
                        }                                                                                \
                        (_pstart)[_j] = _ex;                                                             \
                }                                                                                        \
        }                                                                                                \
}

/*
 * <array> public
 */
#define array_sort_stable(_self, f_Is_Less_Than)                                                         \
{                                                                                                        \
        __array_insert_sort(_self, (_self)->content, (_self)->end, f_Is_Less_Than);                      \
}

/*
 * <array> private
 */
#define __array_bipartition(_self, _lo, _hi, _pivot_value, f_Is_Less_Than, _p)                           \
{                                                                                                        \
        typeof((_self)->content) _low = (_lo);                                                           \
        typeof((_self)->content) _high = (_hi);                                                          \
        while (true) {                                                                                   \
                while (_low != _high && f_Is_Less_Than(_low, &(_pivot_value), (_self)->data)) {          \
                        _low ++;                                                                         \
                }                                                                                        \
                if (_low == _high) {                                                                     \
                        break;                                                                           \
                }                                                                                        \
                do {                                                                                     \
                        _high --;                                                                        \
                } while (_low != _high && !f_Is_Less_Than(_high, &(_pivot_value), (_self)->data));       \
                if (_low == _high) {                                                                     \
                        break;                                                                           \
                }                                                                                        \
                __swap(*_low, *_high);                                                                   \
                _low ++;                                                                                 \
        }                                                                                                \
        (_p) = _low;                                                                                     \
}

#define __array_select_pivot(_self, _left, _right, f_Is_Less_Than, _return_pivot)                        \
{                                                                                                        \
        int _mid = ((_left) + (_right)) >> 1;                                                            \
        /* if (mid < left) */                                                                            \
        if (f_Is_Less_Than(&(_self)->content[_mid],                                                      \
                           &(_self)->content[_left],                                                     \
                            (_self)->data)) {                                                            \
                /* if (mid > right) */                                                                   \
                if (f_Is_Less_Than(&(_self)->content[_right],                                            \
                                   &(_self)->content[_mid],                                              \
                                    (_self)->data)) {                                                    \
                        (_return_pivot) = (_mid);                                                        \
                /* else if (right < left) */                                                             \
                } else if (f_Is_Less_Than(&(_self)->content[_right],                                     \
                                          &(_self)->content[_left],                                      \
                                           (_self)->data)) {                                             \
                        (_return_pivot) = (_right);                                                      \
                } else {                                                                                 \
                        (_return_pivot) = (_left);                                                       \
                }                                                                                        \
        } else {                                                                                         \
                /* if (right > left) */                                                                  \
                if (f_Is_Less_Than(&(_self)->content[_right],                                            \
                                   &(_self)->content[_left],                                             \
                                    (_self)->data)) {                                                    \
                        (_return_pivot) = (_left);                                                       \
                /* else if (mid > right) */                                                              \
                } else if (f_Is_Less_Than(&(_self)->content[_right],                                     \
                                          &(_self)->content[_mid],                                       \
                                           (_self)->data)) {                                             \
                        (_return_pivot) = (_right);                                                      \
                } else {                                                                                 \
                        (_return_pivot) = (_mid);                                                        \
                }                                                                                        \
        }                                                                                                \
}

/* constitute the right part of the array of currect hierachy in stack memory */
struct __qsort_part {
        int     right;
        int     mid_pivot;
};

#define __qsort_push(_stack, _iframe, _right, _pivot)                                                    \
{                                                                                                        \
        (_stack)[_iframe].right = (_right);                                                              \
        (_stack)[_iframe].mid_pivot = (_pivot);                                                          \
        (_iframe) ++;                                                                                    \
}

#define __qsort_pop(_stack, _iframe, _left, _right)                                                      \
{                                                                                                        \
        (_iframe) --;                                                                                    \
        (_left)  = (_stack)[_iframe].mid_pivot + 1;                                                      \
        (_right) = (_stack)[_iframe].right;                                                              \
}

#define __c_MaxStackSize                64
#define __c_CutoffSize                  16

#define __swap(_x, _y)                                                                                   \
{                                                                                                        \
        typeof(_x) _t = (_x);                                                                            \
        (_x) = (_y);                                                                                     \
        (_y) = _t;                                                                                       \
}

/*
 * <array> public
 */
#define array_sort_unstable(_self, f_Is_Less_Than)                                                       \
{                                                                                                        \
        struct __qsort_part     _stack[__c_MaxStackSize];                                                \
                                                                                                         \
                                                                                                         \
        int _iframe     = 0; /* stack frame */                                                           \
        int _left       = 0,                                                                             \
            _right      = (_self)->end - (_self)->content - 1;                                           \
        const int _boundary = _right;                                                                    \
        int _pivot;                                                                                      \
        __array_select_pivot(_self, _left, _right, f_Is_Less_Than, _pivot);                              \
                                                                                                         \
        while (true) {                                                                                   \
                if (_right - _left < __c_CutoffSize) {                                                   \
                        if (_boundary - _left < __c_CutoffSize) {                                        \
                                __array_insert_sort(_self,                                               \
                                                    &(_self)->content[_left],                            \
                                                    &(_self)->content[_boundary + 1],                    \
                                                     f_Is_Less_Than);                                    \
                                break;                                                                   \
                        }                                                                                \
                        __array_insert_sort(_self,                                                       \
                                            &(_self)->content[_left],                                    \
                                            &(_self)->content[_right + 1],                               \
                                             f_Is_Less_Than);                                            \
                        __qsort_pop(_stack, _iframe, _left, _right);                                     \
                        __array_select_pivot(_self, _left, _right, f_Is_Less_Than, _pivot);              \
                        continue;                                                                        \
                } else {                                                                                 \
                        typeof (*(_self)->content) _pivot_value = (_self)->content[_pivot];              \
                        __swap((_self)->content[_pivot], (_self)->content[_right]);                      \
                                                                                                         \
                        typeof((_self)->content) _ptr;                                                   \
                        __array_bipartition(_self, &(_self)->content[_left], &(_self)->content[_right],  \
                                            _pivot_value, f_Is_Less_Than, _ptr);                         \
                        int _new_pivot = _ptr - (_self)->content;                                        \
                                                                                                         \
                        __swap((_self)->content[_right], (_self)->content[_new_pivot]);                  \
                                                                                                         \
                        __qsort_push(_stack, _iframe, _right, _new_pivot);                               \
                        _right = _new_pivot;                                                             \
                        __array_select_pivot(_self, _left, _right, f_Is_Less_Than, _pivot);              \
                }                                                                                        \
        }                                                                                                \
}


#endif	// ARRAY_H_INCLUDED
