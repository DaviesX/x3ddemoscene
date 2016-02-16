#ifndef QUICKSORT_H_INCLUDED
#define QUICKSORT_H_INCLUDED


#include <container/container.h>

PseudoDef(void alg_bubble_sort(void* list, int len, void* info,
                                bool (*cmp) (void* elm0, void* elm1, void* info));)
PseudoDef(void alg_insert_sort(void* list, int len, void* info,
                                bool (*cmp) (void* elm0, void* elm1, void* info));)
PseudoDef(void alg_quick_sort(void* list, int len, void* info,
                               bool (*cmp) (void* elm0, void* elm1, void* info));)
PseudoDef(void alg_split_bymidvalue(void* list, int len, TYPE mid_val,
                                     bool (*cmp) (TYPE mid_val, void* elm), int* i_result);)
PseudoDef(void alg_split_byvalue(void* array, int len, void* info,
                                  bool (*cmp) (void* info, void* elm),
                                  int* i_result);)
PseudoDef(void alg_split_byindex(void* array, int len, int i, void* info,
                                  bool (*cmp) (void* elm0, void* elm1, void* info));)


#define alg_bubble_sort( list_, len_, info_, _f_Cmp ) \
{ \
        int i_ = (len_) - 1; \
        while ( i_ > 0 ) { \
                int last_ = 0; \
                int j_; \
                for ( j_ = 0; j_ < _i; j_ ++ ) { \
                        if ( _f_Cmp ( &(list_)[j_ + 1], &(list_)[j_] ), (info_) ) { \
                                typeof (*(list_)) t_ =  (list_)[j_]; \
                                (list_)[j_] = (list_)[j_ + 1]; \
                                (list_)[j_ + 1] = t_; \
                                last_ = j_; \
                        } \
                } \
                i_ = last_; \
        } \
}

#define alg_insert_sort( list_, len_, info_, _f_Cmp ) \
{ \
        int i_; \
        for ( i_ = 0; i_ < (len_); i_ ++ ) { \
                if ( _f_Cmp ( &(list_)[i_], &(list_)[i_ - 1], info_ ) ) { \
                        typeof (*(list_)) ex_ = (list_)[i_]; \
                        int j_; \
                        for ( j_ = i_; j_ > 0; j_ -- ) { \
                                if ( _f_Cmp ( &ex_, &(list_)[j_ - 1], info_ ) ) { \
                                        (list_)[j_] = (list_)[j_ - 1]; \
                                } else { \
                                        break; \
                                } \
                        } \
                        (list_)[j_] = ex_; \
                } \
        } \
}

#define pivot( list_, left_, right_, pivot_, info_, _f_Cmp ) \
{ \
        int mid_ = ((left_) + (right_)) >> 1; \
        /* if ( left_ > mid_ ) */ \
        if ( _f_Cmp ( &(list_)[mid_], &(list_)[left_], info_ ) ) { \
                /* if ( mid_ > right_ ) */ \
                if ( _f_Cmp ( &(list_)[right_], &(list_)[mid_], info_ ) ) { \
                        (pivot_) = mid_; \
                /* else if ( left_ > right_ ) */ \
                } else if ( _f_Cmp ( &(list_)[right_], &(list_)[left_], info_ ) ) { \
                        (pivot_) = right_; \
                } else { \
                        (pivot_) = left_; \
                } \
        } else { \
                /* if ( left_ > right_ ) */ \
                if ( _f_Cmp ( &(list_)[right_], &(list_)[left_], info_ ) ) { \
                        (pivot_) = left_; \
                /* else if ( mid_ > right_ ) */ \
                } else if ( _f_Cmp ( &(list_)[right_], &(list_)[mid_], info_ ) ) { \
                        (pivot_) = (right_); \
                } else { \
                        (pivot_) = (mid_); \
                } \
        } \
}

/* constitute the right part of the array of currect hierachy */
struct qsort_part {
        int right;
        int mid_pivot;
};

#define qsort_push( _stack, _iframe, _right, _pivot ) \
{ \
        (_stack)[_iframe].right = (_right); \
        (_stack)[_iframe].mid_pivot = (_pivot); \
        (_iframe) ++; \
}

#define qsort_pop( _stack, _iframe, _left, _right ) \
{ \
        (_iframe) --; \
        (_left)  = (_stack)[_iframe].mid_pivot + 1; \
        (_right) = (_stack)[_iframe].right; \
}

#define MAX_ITERATIONS		64
#define CUT_OFF_SIZE		16

/* generic quick sort algorithm */
#define alg_quick_sort( _list, _length, _info, _f_Cmp ) \
{\
        struct qsort_part _stack[MAX_ITERATIONS];\
\
        int _iframe = 0; /* stack frame */\
        int _left = 0, \
            _right = (_length) - 1;\
        const int _boundary = _right;\
        int _i_pivot;\
        pivot ( _list, 0, _right, _i_pivot, _info, _f_Cmp );\
\
        while ( true ) {\
                if ( _right - _left < CUT_OFF_SIZE ) {\
                        if ( _boundary - _left < CUT_OFF_SIZE ) { \
                                alg_insert_sort ( &(_list)[_left], \
                                                  _boundary - _left + 1, \
                                                  _info, _f_Cmp ); \
                                break;\
                        }\
                        alg_insert_sort ( &(_list)[_left], \
                                          _right - _left + 1,\
                                          _info, _f_Cmp );\
                        qsort_pop ( _stack, _iframe, _left, _right );\
                        pivot ( _list, _left, _right, _i_pivot, _info, _f_Cmp );\
                        continue;\
                } else {\
                        typeof (*(_list)) _pivot_value = (_list)[_i_pivot]; \
                        typeof (*(_list)) _t = (_list)[_i_pivot]; \
                        (_list)[_i_pivot] = (_list)[_right]; \
                        (_list)[_right] = _t; \
                        int _new_i_pivot = _left;\
\
                        int _i;\
                        for ( _i = _left; _i < _right; _i ++ ) {\
                                if ( _f_Cmp ( &(_list)[_i], &_pivot_value, _info ) ) {\
                                        typeof (*(_list)) _t = (_list)[_new_i_pivot]; \
                                        (_list)[_new_i_pivot] = (_list)[_i]; \
                                        (_list)[_i] = _t; \
                                        _new_i_pivot ++;\
                                }\
                        }\
\
                        _t = (_list)[_new_i_pivot]; \
                        (_list)[_new_i_pivot] = (_list)[_right]; \
                        (_list)[_right] = _t; \
\
                        qsort_push ( _stack, _iframe, _right, _new_i_pivot );\
                        _right = _new_i_pivot;\
                        pivot ( _list, _left, _right, _i_pivot, _info, _f_Cmp );\
                }\
        }\
}

/* partition an array list in-place according to the given mid-value */
#define alg_split_bymidvalue( _list, _len, _mval, _f_Cmp, _i_result ) \
{ \
        int _new_i_pivot = 0; \
        int _i; \
        for ( _i = 0; _i < (_len); _i ++ ) { \
                if ( _f_Cmp ( (_mval), &(_list)[_i] ) ) { \
                        typeof (*(_list)) _t = (_list)[_new_i_pivot]; \
                        (_list)[_new_i_pivot] = (_list)[_i]; \
                        (_list)[_i] = _t; \
                        _new_i_pivot ++; \
                } \
        } \
        *(_i_result) = _new_i_pivot; \
}

/* partition an array list in-place according to the custum mid-value */
#define alg_split_byvalue( _array, _len, _info, _f_Cmp, _i_result ) \
{ \
        int _pivot = 0; \
        int _i; \
        for ( _i = 0; _i < (_len); _i ++ ) { \
                if ( _f_Cmp ( _info, &(_array)[_i] ) ) { \
                        /* Swap */ \
                        typeof (*(_array)) _t = (_array)[_i]; \
                        (_array)[_i] = (_array)[_pivot]; \
                        (_array)[_pivot] = _t; \
			_pivot ++; \
                }\
        }\
	*(_i_result) = _pivot;\
}

/* split an array into two part with middle at the ith position */
#define alg_split_byindex( _array, _len, _i_mid, _info, _f_Cmp )	\
{ \
        int _left = 0, \
            _right = (_len) - 1; \
        typeof (*(_array)) _pivot_value; \
        int _i_pivot = 0; \
        pivot ( _array, 0, _right, _i_pivot, _info, _f_Cmp ); \
        _pivot_value = (_array)[_i_pivot]; \
\
        while ( true ) { \
                int _part_pos = _left; \
\
                typeof (*(_array)) _t; \
                _t = (_array)[_i_pivot]; \
                (_array)[_i_pivot] = (_array)[_right]; \
                (_array)[_right] = _t; \
\
                int _i; \
                for ( _i = _left; _i < _right; _i ++ ) { \
                        if ( _f_Cmp ( &(_array)[_i], &_pivot_value, _info ) ) { \
                                _t = (_array)[_part_pos]; \
                                (_array)[_part_pos] = (_array)[_i]; \
                                (_array)[_i] = _t; \
                                _part_pos ++; \
                        } \
                } \
\
                _t = (_array)[_right]; \
                (_array)[_right] = (_array)[_part_pos]; \
                (_array)[_part_pos] = _t; \
\
                if ( _part_pos > _i_mid ) { \
                        _right = _part_pos - 1; \
                } else if ( _part_pos < _i_mid ) { \
                        _left = _part_pos + 1; \
                        _i_pivot = _left; \
                } else { \
                        break; \
                } \
\
                if ( _right - _left <= CUT_OFF_SIZE ) { \
                        alg_insert_sort ( &(_array)[_left], \
                                           _right - _left + 1, \
                                           (_info), _f_Cmp ); \
                        break; \
                } \
\
                pivot ( _array, _left, _right, _i_pivot, _info, _f_Cmp ); \
                _pivot_value = (_array)[_i_pivot]; \
        } \
}


#endif // QUICKSORT_H_INCLUDED
