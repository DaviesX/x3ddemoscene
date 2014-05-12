/* algorithm: Some commonly used algorithm which is implemented in the header
 * to avoid function pointer also to take advantage of generic type */
// #ifndef ALGORITHM_INC_INCLUDED
// #define ALGORITHM_INC_INCLUDED


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

#define create_alg_ptr_list( _list, _len, _is_inplace, _ptrs ) \
{ \
        int _ptr_len = (_len)*(_is_inplace + 1); \
        (_ptrs)->ptr = alloc_fix ( sizeof *(_ptrs), _ptr_len );\
        int _i; \
        for ( _i = 0; _i < (_len); _i ++ ) { \
                (_ptrs)[_i].ptr = &(_list)[_i]; \
        } \
}

#define free_alg_ptr_list( _ptrs )         (free_fix ( _ptrs ))

#define alg_ptr_list_tonew( _ptrs, _len, _new_list ) \
{ \
        int _i; \
        for ( _i = 0; _i < (_len); _i ++ ) { \
                _new_list[_i] = *(cast(_new_list) (_ptrs)[_i].ptr); \
        } \
}

#define alg_ptr_list_inplace( _ptr_list, _len, _list ) \
{ \
        typeof(&_list) _ori_ptrs = cast(&_list) &(_ptr_list)[_len]; \
        typeof(&_list) _ptrs = cast(&_list) &(_ptr_list)[0]; \
        int _i; \
        for ( _i = 0; _i < (_len); _i ++ ) { \
                _ori_ptrs[(_ptrs)[_i] - (_list)] = &_list[_i]; \
        } \
        typeof (*(_list)) _temp[2]; \
        _temp[0] = (_list)[0]; \
\
        int _i_iter = 0; \
        int _i_elm = 0; \
        int _switcher = 0; \
        while ( _i_elm < (_len) ) { \
                if (  (_ptrs)[_i_iter] != &(_list)[_i_iter] ) { \
                        (_ptrs)[_i_iter] = &(_list)[_i_iter]; \
                        _i_iter = (_ptrs)[_i_iter] - (_list); \
                        _temp[(_switcher + 1) & 1] = (_list)[_i_iter]; \
                        (_list)[_i_iter] = _temp[_switcher & 1]; \
                        _switcher ++; \
                } else { \
                        do { \
                                _i_iter ++; \
                                _i_elm ++; \
                        } while ( &(_list)[_i_iter] == (_ptrs)[_i_iter] ); \
                        _temp[_switcher & 1] = (_list)[_i_iter]; \
                } \
        } \
        free_fix ( _ptrs ); \
}

/* Generate a 32 bit hash key for 32 and 64 bits value */
#define gen_hash32( _value )				((*(unsigned int *) &(_value))*0x9e370001UL)
#define gen_hash64( _value )				((*(uint64_t *) &(_value))*0x9e37fffffffc0001UL)

#define HASH_NULL_KEY		-1


/* create and initialize the hash linked list */
#define create_alg_hash_llist( _header_len, _n_elm, _elm_size, _hash_llist ) \
{ \
	int _power = log2_int_c ( (_header_len) ); \
	int _table_len = 1 << _power; \
	(_hash_llist)->hash_prev = alloc_var ( \
                sizeof *(_hash_llist)->hash_prev, _table_len + (_n_elm) + 1 ); \
        (_hash_llist)->hash_prev = &(_hash_llist)->hash_prev[1]; /* so that even (_hash_llist)->hash_prev[HASH_NULL_KEY] memory address will be available */ \
        (_hash_llist)->next      = alloc_var ( \
                sizeof *(_hash_llist)->hash_prev, _table_len + (_n_elm) ); \
        (_hash_llist)->icurr = _table_len + _n_elm; \
        (_hash_llist)->content = alloc_var ( _elm_size, _n_elm ); \
	(_hash_llist)->shift_bits = 32 - (_power); \
	(_hash_llist)->size_bits = _table_len - 1; \
	(_hash_llist)->header_len = _table_len; \
	(_hash_llist)->n_elm = (_n_elm); \
	int _i; \
	for ( _i = 0; _i < _table_len + (_n_elm); _i ++ ) { \
		(_hash_llist)->hash_prev[_i] = HASH_NULL_KEY; \
		(_hash_llist)->next[_i] = HASH_NULL_KEY; \
	} \
}

#define free_alg_hash_llist( _hash_llist ) \
{ \
        (_hash_llist)->hash_prev = &(_hash_llist)->hash_prev[-1]; \
	free_var ( (_hash_llist)->hash_prev ); \
	free_var ( (_hash_llist)->next ); \
	free_var ( (_hash_llist)->content ); \
	memset ( _hash_llist, 0, sizeof *(_hash_llist) ); \
}

#define alg_hash_llist_flush( _hash_llist ) \
{ \
	flush_var ( (_hash_llist)->hash_prev ); \
	flush_var ( (_hash_llist)->next ); \
	flush_var ( (_hash_llist)->content ); \
\
	int _n_elm = (_hash_llist)->header_len + \
		     (_hash_llist)->n_elm; \
	int _i; \
	for ( _i = 0; _i < _n_elm; _i ++ ) { \
		(_hash_llist)->hash_prev[_i] = HASH_NULL_KEY; \
		(_hash_llist)->next[_i] = HASH_NULL_KEY; \
	} \
}

/* expand the memory if needs a dynamic hash table */
#define f_No_Update( _hash_llist, _i )

#define f_Dym_Update( _hash_llist, _i ) \
{ \
        const int _new_cnt = (_i) + 1; \
        if ( _new_cnt > 2*(_hash_llist)->n_elm ) { \
                /* used up all the space, we needs to expand and partially \
                 * initialize those new spaces */ \
                int _len = (_hash_llist)->header_len + _new_cnt; \
                (_hash_llist)->hash_prev = expand2_var ( (_hash_llist)->hash_prev, _len ); \
                (_hash_llist)->next      = expand2_var ( (_hash_llist)->next, _len ); \
                (_hash_llist)->content = expand2_var ( (_hash_llist)->content, _new_cnt ); \
\
                int *_hash_prev = &(_hash_llist)->hash_prev[(_hash_llist)->header_len]; \
                int *_next = &(_hash_llist)->next[(_hash_llist)->header_len]; \
                for ( ; (_hash_llist)->n_elm < 2*_new_cnt; \
                        (_hash_llist)->n_elm ++ ) { \
                        _hash_prev[(_hash_llist)->n_elm] = HASH_NULL_KEY; \
                        _next[(_hash_llist)->n_elm] = HASH_NULL_KEY; \
                } \
        } \
}

#define hash_update_icurr( _hash_llist ) \
{ \
        (_hash_llist)->icurr = \
                ((_hash_llist)->next[(_hash_llist)->icurr] == HASH_NULL_KEY ? \
                 (_hash_llist)->icurr + 1 : \
                 (_hash_llist)->next[(_hash_llist)->icurr]); \
}

/* add an element to the hash linked list (Note that: addition is different from insertion) */
#define alg_hash_llist_add( _hash_llist, _elm, _key, _f_Update ) \
{ \
        int _head_pos = (uint64_t) _key & (_hash_llist)->size_bits; \
        if ( (_hash_llist)->next[_head_pos] == HASH_NULL_KEY ) { \
                /* insert new entry into the hash table */ \
                (_hash_llist)->next[_head_pos] = (_hash_llist)->icurr; \
                (_hash_llist)->hash_prev[(_hash_llist)->icurr] = _head_pos; \
                typeof (_elm) dest = cast(_elm) (_hash_llist)->content + \
                                     ((_hash_llist)->icurr - (_hash_llist)->header_len); \
                *dest = *(_elm); \
                hash_update_icurr ( _hash_llist ); \
                _f_Update ( _hash_llist, (_hash_llist)->icurr ); \
        } else { \
                /* append new entry into the linked list */ \
                int _icurr = (_hash_llist)->next[_head_pos]; \
                int *_next = (_hash_llist)->next; \
                while ( _next[_icurr] != HASH_NULL_KEY ) { \
                        _icurr = _next[_icurr]; \
                } \
                (_hash_llist)->next[_icurr] = (_hash_llist)->icurr; \
                (_hash_llist)->hash_prev[(_hash_llist)->icurr] = _icurr; \
                typeof (_elm) dest = cast(_elm) (_hash_llist)->content + \
                                     ((_hash_llist)->icurr - (_hash_llist)->header_len); \
                *dest = *(_elm); \
                hash_update_icurr ( _hash_llist ); \
                _f_Update ( _hash_llist, (_hash_llist)->icurr ); \
        } \
}

/* find the first matched occurrence from hash linked list
 * and return the pointer to that element, nullptr is guaranteed
 * if we can't find a matching element at all */
#define alg_hash_llist_first( _hash_llist, _key, _rfound, _info, _f_Cmp ) \
{ \
        int _head_pos = (uint64_t) _key & (_hash_llist)->size_bits; \
        if ( (_hash_llist)->next[_head_pos] == HASH_NULL_KEY ) { \
                *(_rfound) = nullptr; \
        } else { \
                int _curr = (_hash_llist)->next[_head_pos]; \
                const int *_next = (_hash_llist)->next; \
                typeof (*(_rfound)) _ptr = (_hash_llist)->content; \
                _ptr -= (_hash_llist)->header_len; \
\
                while ( true ) { \
                        if ( _f_Cmp ( _info, &_ptr[_curr] ) ) { \
                                /* matched */ \
                                *(_rfound) = &_ptr[_curr]; \
                                break; \
                        } else { \
                                if ( _next[_curr] != HASH_NULL_KEY ) { \
                                        /* Go on to the next element */ \
                                        _curr = _next[_curr]; \
                                } else { \
                                        *(_rfound) = nullptr; \
                                        break; \
                                } \
                        } \
                } \
        } \
}

/* find the next matched occurrence from hash linked list
 * and return the pointer to that element, nullptr is guaranteed
 * if we can't find a matching element at all */
#define alg_hash_llist_next( _hash_llist, _rfound, _info, _f_Cmp ) \
{ \
	const int *_next = (_hash_llist)->next; \
	typeof (*(_rfound)) _ptr = (_hash_llist)->content; \
	_ptr -= (_hash_llist)->header_len; \
	int _curr = *(_rfound) - _ptr; \
\
	while ( true ) { \
		if ( _next[_curr] != HASH_NULL_KEY ) { \
			/* Go on to the next element */ \
			_curr = _next[_curr]; \
			if ( _f_Cmp ( _info, &_ptr[_curr] ) ) { \
				*(_rfound) = &_ptr[_curr]; \
				break; \
			} \
		} else { \
			*(_rfound) = nullptr; \
			break; \
		} \
	} \
}

/* insert an element to the position obtained from
 *alg_hash_llist_first/alg_hash_llist_next */
#define alg_hash_llist_insert( _hash_llist, _elm, _rfound ) \
{ \
        typeof (*(_rfound)) _ptr = (_hash_llist)->content; \
	_ptr -= (_hash_llist)->header_len; \
	int _curr = *(_rfound) - _ptr; \
\
	/* force the found element to use a new position \
	 * but keep the linkage valid */ \
        int *_prev = (_hash_llist)->hash_prev; \
	int *_next = (_hash_llist)->next; \
	int _new_pos = (_hash_llist)->icurr; \
	hash_update_icurr ( _hash_llist ); \
	_prev[_next[_curr]] = _new_pos; \
	_next[_curr] = _new_pos; \
	_ptr[_new_pos] = _ptr[_curr]; \
	/* copy the element into this position */ \
	_ptr[_curr] = *(_elm); \
}

/* delete an element obtained from alg_hash_llist_first/alg_hash_llist_next */
#define alg_hash_llist_remove( _hash_llist, _rfound ) \
{ \
        typeof (*(_rfound)) _ptr = (_hash_llist)->content; \
	_ptr -= (_hash_llist)->header_len; \
	int _curr = *(_rfound) - _ptr; \
\
        /* remove the current, also to fix the linkage of \
         * the previous and the next element */ \
	int *_prev = (_hash_llist)->hash_prev; \
	int *_next = (_hash_llist)->next; \
        _next[_prev[_curr]] = _next[_curr]; \
        _prev[_next[_curr]] = _prev[_curr]; \
        /* append itself behind the icurr position */ \
        int _icurr = (_hash_llist)->icurr; \
        _prev[_next[_icurr]] = _curr; \
        _next[_curr] = _next[_icurr]; \
        _next[_icurr] = _curr; \
        _prev[_curr] = _icurr; \
}

/** list containers **/
#define list_elm( _list, _i )			(&(_list)->list[(_i)*(_list)->elm_size])

#define alg_list_find( _list, _info, _ret_pos, _compare_func )	\
{\
	*(_ret_pos) = -1; \
	int _i; \
	for ( _i = 0; _i < (_list)->num_elm; _i ++ ) { \
		if ( _compare_func ( (_info), list_elm(_list, _i) ) ) { \
			*(_ret_pos) = _i; \
			break; \
		} \
	} \
}

#define alg_list_remove( _list, _info, _compare_func ) \
{ \
	int _i; \
	for ( _i = 0; _i < (_list)->num_elm; _i ++ ) { \
		if ( _compare_func ( (_info), list_elm(_list, _i) ) ) { \
			break; \
		} \
	} \
	if ( _i != (_list)->num_elm ) { \
		if ( (_list)->ops.destroy_callback ) { \
			(_list)->ops.destroy_callback ( list_elm( _list, _i ) ); \
		} \
		(_list)->num_elm --; \
		memcpy ( &(_list)->list[_i], &(_list)->list[(_list)->num_elm], \
			(_list)->elm_size ); \
	} \
}

#define alg_list_n( _list )			((_list)->num_elm)
#define alg_list_first( _list )			((void *) (_list)->list)
#define alg_list_next( _list_elm )		((_list_elm) ++)
#define alg_list_i( _list, _k, _ptr )           (*(_ptr) = &((typeof(*(_ptr))) (_list)->list)[_k])
#define alg_list_locate( _elm, _list )          ((_elm) - (cast(_elm) (_list)->list))

#define alg_llist_add( _elm, _llist ) \
{ \
        int _i = (_llist)->icurr; \
        (_llist)->prev = expand2_var ( (_llist)->prev, (_llist)->num_elm ); \
        (_llist)->head = (_llist)->prev; \
        (_llist)->next = expand2_var ( (_llist)->next, (_llist)->num_elm ); \
        (_llist)->content = expand2_var ( (_llist)->content, (_llist)->num_elm ); \
        typeof(_elm) _dest = (typeof(_elm)) (_llist)->content + _i; \
        memcpy ( _dest, _elm, sizeof(*(_elm)) ); \
        if ( (_llist)->icurr == (_llist)->ilast ) { \
                (_llist)->next[_i] = ++ (_llist)->ilast; \
                (_llist)->prev[(_llist)->next[_i]] = _i; \
        } \
        (_llist)->icurr = (_llist)->next[_i]; \
        (_llist)->num_elm ++; \
}

#define alg_llist_find( _llist, _info, _elm, _f_Compare ) \
{ \
        *(_elm) = nullptr; \
        int _curr = (_llist)->head[0]; \
        typeof(*(_elm)) _ptr = (_llist)->content; \
        while ( _curr != (_llist)->icurr ) { \
                if ( _f_Compare ( _info, &_ptr[_curr] ) ) { \
                        *(_elm) = &_ptr[_curr]; \
                        break; \
                } \
                _curr = (_llist)->next[_curr]; \
        } \
}

#define alg_llist_remove_i( _i, _llist ) \
{ \
        (_llist)->prev[(_llist)->next[(_i)]] = (_llist)->prev[(_i)]; \
        (_llist)->next[(_llist)->prev[(_i)]] = (_llist)->next[(_i)]; \
        if ( (_llist)->icurr != (_llist)->ilast ) \
                (_llist)->prev[(_llist)->next[(_llist)->icurr]] = (_i); \
        (_llist)->next[(_llist)->icurr] = (_i); \
        (_llist)->num_elm --; \
}

#define alg_llist_remove( _llist, _info, _elm, _f_Compare ) \
{ \
        *(_elm) = nullptr; \
        int _curr = (_llist)->head[0]; \
        typeof(*(_elm)) _ptr = (_llist)->content; \
        while ( _curr != (_llist)->icurr ) \
        { \
                if ( _f_Compare ( _info, &_ptr[_curr] ) ) { \
                        *(_elm) = &_ptr[_curr]; \
                        alg_llist_remove_i ( _curr, _llist ); \
                        break; \
                } \
                _curr = (_llist)->next[_curr]; \
        } \
}

#define alg_llist_remove_ptr( _elm, _llist ) \
{ \
        typeof(_elm) _ptr = (_llist)->content; \
        int _i = (_elm) - _ptr; \
        alg_llist_remove_i ( _i, _llist ); \
}

#define alg_llist_i( _llist, _i, _elm ) \
{ \
        *(_elm) = (typeof(*(_elm))) (_llist)->content + (_i); \
}

#define alg_llist_locate( _elm, _llist )                ((_elm) - cast(_elm) (_llist)->content)

#define alg_llist_push                  alg_llist_add

#define alg_llist_pop( _llist, _elm ) \
{ \
        *(_elm) = nullptr; \
        if ( (_llist)->icurr != (_llist)->head[0] ) { \
                int _i = (_llist)->prev[(_llist)->icurr]; \
                typeof(*(_elm)) _ptr = (_llist)->content; \
                *(_elm) = &_ptr[_i]; \
                alg_llist_remove_i ( _i, _llist ); \
        } \
}


// #endif // ALGORITHM_INC_INCLUDED
