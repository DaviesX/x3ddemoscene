#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED


#include <container/container.h>
#include <math/math.h>

#pragma pack(4)

/* Provides a linked list hash table container
 * Some common O(1) operations are defined below */
struct alg_hash_llist {
        void*           content;
        int*            hash_prev;      /* Table header and the followed prev linked list */
        int*            next;           /* next linked list */
        int             icurr;
        int             header_len;
        int             n_elm;
        int             shift_bits;
        uint64_t        size_bits;
};

#pragma pack()


pseudo_def(void create_alg_hash_llist(int header_len, int n_elm, int elm_size,
                                      struct alg_hash_llist* hash_llist);)
pseudo_def(void free_alg_hash_llist(struct alg_hash_llist* hash_llist);)
pseudo_def(void alg_hash_llist_flush(struct alg_hash_llist* hash_llist);)
pseudo_def(typedef (*f_No_Update) (struct alg_hash_llist* hash_llist, int i);)
pseudo_def(typedef (*f_Dym_Update) (struct alg_hash_llist* hash_llist, int i);)
pseudo_def(void alg_hash_llist_add(struct alg_hash_llist* hash_llist, void *elm, uint64_t key, f_Update);)
pseudo_def(void alg_hash_llist_first(struct alg_hash_llist* hash_llist,
                                     uint64_t key, void* rfound, void* info,
                                     bool (*cmp) (void* info, void* elm));)
pseudo_def(void alg_hash_llist_next(struct alg_hash_llist *hash_llist, void *rfound, void* info,
                                    bool (*cmp) (void* info, void *elm));)
pseudo_def(void alg_hash_llist_insert(struct alg_hash_llist* hash_llist, void* elm, void* rfound);)
pseudo_def(void alg_hash_llist_remove(struct alg_hash_llist* hash_llist, void* rfound);)



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


#endif // HASHMAP_H_INCLUDED
