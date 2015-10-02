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


PseudoDef(void alg_hash_llist_init(struct alg_hash_llist* self, int header_len, int n_elm, int elm_size);)
PseudoDef(void alg_hash_llist_free(struct alg_hash_llist* self);)
PseudoDef(void alg_hash_llist_flush(struct alg_hash_llist* self);)
PseudoDef(typedef (*f_No_Update) (struct alg_hash_llist* self, int i);)
PseudoDef(typedef (*f_Dym_Update) (struct alg_hash_llist* self, int i);)
PseudoDef(void alg_hash_llist_add(struct alg_hash_llist* self, void *elm, uint64_t key, f_Update);)
PseudoDef(void alg_hash_llist_first(struct alg_hash_llist* self,
                                    uint64_t key, void* rfound, void* info,
                                    bool (*cmp) (void* info, void* elm));)
PseudoDef(void alg_hash_llist_next(struct alg_hash_llist *self, void *rfound, void* info,
                                   bool (*cmp) (void* info, void* elm));)
PseudoDef(void alg_hash_llist_insert(struct alg_hash_llist* self, void* elm, void* rfound);)
PseudoDef(void alg_hash_llist_remove(struct alg_hash_llist* self, void* rfound);)



/* Generate a 32 bit hash key for 32 and 64 bits value */
#define gen_hash32( _value )				((*(unsigned int *) &(_value))*0x9e370001UL)
#define gen_hash64( _value )				((*(uint64_t *) &(_value))*0x9e37fffffffc0001UL)

/* create and initialize the hash linked list */
#define alg_hash_llist_init(_self, _header_len, _n_elm, _elm_size) \
{ \
	int _power              = log2_int_c(_header_len); \
	int _table_len          = 1 << _power; \
	(_self)->hash_prev      = alloc_var(sizeof(*(_self)->hash_prev), _table_len + (_n_elm) + 1); \
        (_self)->hash_prev      = &(_self)->hash_prev[1]; /* so that even (_self)->hash_prev[NIL_POSITION] memory address will be available */ \
        (_self)->next           = alloc_var(sizeof(*(_self)->hash_prev), _table_len + (_n_elm)); \
        (_self)->icurr          = _table_len + _n_elm; \
        (_self)->content        = alloc_var(_elm_size, _n_elm); \
	(_self)->shift_bits     = 32 - (_power); \
	(_self)->size_bits      = _table_len - 1; \
	(_self)->header_len     = _table_len; \
	(_self)->n_elm          = (_n_elm); \
	int _i; \
	for (_i = 0; _i < _table_len + (_n_elm); _i ++) { \
		(_self)->hash_prev[_i]  = NIL_POSITION; \
		(_self)->next[_i]       = NIL_POSITION; \
	} \
}

#define alg_hash_llist_free(_self)                      \
{                                                       \
        (_self)->hash_prev = &(_self)->hash_prev[-1];   \
	free_var((_self)->hash_prev );                  \
	free_var((_self)->next );                       \
	free_var((_self)->content );                    \
	zero_obj(_self);                                 \
}

#define alg_hash_llist_flush(_self) \
{ \
	alloc_flush_var ( (_self)->hash_prev ); \
	alloc_flush_var ( (_self)->next ); \
	alloc_flush_var ( (_self)->content ); \
\
	int _n_elm = (_self)->header_len + (_self)->n_elm; \
	int _i; \
	for (_i = 0; _i < _n_elm; _i ++) { \
		(_self)->hash_prev[_i] = NIL_POSITION; \
		(_self)->next[_i] = NIL_POSITION; \
	} \
}

/* expand the memory if needs a dynamic hash table */
#define f_No_Update(_self, _i)

#define f_Dym_Update(_self, _i) \
{ \
        const int _new_cnt = (_i) + 1; \
        if ( _new_cnt > 2*(_self)->n_elm ) { \
                /* used up all the space, we needs to expand and partially \
                 * initialize those new spaces */ \
                int _len                = (_self)->header_len + _new_cnt; \
                (_self)->hash_prev      = alloc_expand2_var((_self)->hash_prev, _len); \
                (_self)->next           = alloc_expand2_var((_self)->next, _len); \
                (_self)->content        = alloc_expand2_var((_self)->content, _new_cnt); \
\
                int *_hash_prev         = &(_self)->hash_prev[(_self)->header_len]; \
                int *_next              = &(_self)->next[(_self)->header_len]; \
                for (; (_self)->n_elm < 2*_new_cnt; (_self)->n_elm ++) { \
                        _hash_prev[(_self)->n_elm]      = NIL_POSITION; \
                        _next[(_self)->n_elm]           = NIL_POSITION; \
                } \
        } \
}

#define hash_update_icurr( _self ) \
{ \
        (_self)->icurr = \
                ((_self)->next[(_self)->icurr] == NIL_POSITION ? \
                 (_self)->icurr + 1 : \
                 (_self)->next[(_self)->icurr]); \
}

/* add an element to the hash linked list (Note that: addition is different from insertion) */
#define alg_hash_llist_add(_self, _elm, _key, _f_Update) \
{ \
        int _head_pos = (uint64_t) _key & (_self)->size_bits; \
\
        if ((_self)->next[_head_pos] == NIL_POSITION) { \
                /* insert new entry into the hash table */ \
                (_self)->next[_head_pos]                = (_self)->icurr; \
                (_self)->hash_prev[(_self)->icurr]      = _head_pos; \
                typeof (_elm) dest                      = cast(_elm) (_self)->content + ((_self)->icurr - (_self)->header_len); \
                *dest                                   = *(_elm); \
                hash_update_icurr(_self); \
                _f_Update(_self, (_self)->icurr); \
        } else { \
                /* append new entry into the linked list */ \
                int _icurr = (_self)->next[_head_pos]; \
                int *_next = (_self)->next; \
                while (_next[_icurr] != NIL_POSITION) { \
                        _icurr = _next[_icurr]; \
                } \
                (_self)->next[_icurr]                   = (_self)->icurr; \
                (_self)->hash_prev[(_self)->icurr]      = _icurr; \
                typeof (_elm) dest                      = cast(_elm) (_self)->content + ((_self)->icurr - (_self)->header_len); \
                *dest                                   = *(_elm); \
                hash_update_icurr(_self); \
                _f_Update(_self, (_self)->icurr); \
        } \
}

/* find the first matched occurrence from hash linked list
 * and return the pointer to that element, nullptr is guaranteed
 * if we can't find a matching element at all */
#define alg_hash_llist_first(_self, _key, _rfound, _info, _f_Cmp) \
{ \
        int _head_pos = (uint64_t) _key & (_self)->size_bits; \
\
        if ((_self)->next[_head_pos] == NIL_POSITION) { \
                *(_rfound) = nullptr; \
        } else { \
                int _curr                       = (_self)->next[_head_pos]; \
                const int *_next                = (_self)->next; \
                typeof (*(_rfound)) _ptr        = (_self)->content; \
                _ptr                            -= (_self)->header_len; \
\
                while (true) { \
                        if (_f_Cmp( _info, &_ptr[_curr])) { \
                                /* matched */ \
                                *(_rfound) = &_ptr[_curr]; \
                                break; \
                        } else { \
                                if (_next[_curr] != NIL_POSITION) { \
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
#define alg_hash_llist_next(_self, _rfound, _info, _f_Cmp) \
{ \
	const int *_next                = (_self)->next; \
	typeof (*(_rfound)) _ptr        = (_self)->content; \
	_ptr                            -= (_self)->header_len; \
	int _curr                       = *(_rfound) - _ptr; \
\
	while (true) { \
		if (_next[_curr] != NIL_POSITION ) { \
			/* Go on to the next element */ \
			_curr = _next[_curr]; \
			if (_f_Cmp(_info, &_ptr[_curr])) { \
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
#define alg_hash_llist_insert(_self, _elm, _rfound) \
{ \
        typeof (*(_rfound)) _ptr        = (_self)->content; \
	_ptr                            -= (_self)->header_len; \
	int _curr                       = *(_rfound) - _ptr; \
\
	/* force the found element to use a new position \
	 * but keep the linkage valid */ \
        int *_prev              = (_self)->hash_prev; \
	int *_next              = (_self)->next; \
	int _new_pos            = (_self)->icurr; \
	hash_update_icurr(_self); \
	_prev[_next[_curr]]     = _new_pos; \
	_next[_curr]            = _new_pos; \
	_ptr[_new_pos]          = _ptr[_curr]; \
	/* copy the element into this position */ \
	_ptr[_curr]             = *(_elm); \
}

/* delete an element obtained from alg_hash_llist_first/alg_hash_llist_next */
#define alg_hash_llist_remove(_self, _rfound) \
{ \
        typeof (*(_rfound)) _ptr        = (_self)->content; \
	_ptr                            -= (_self)->header_len; \
	int _curr                       = *(_rfound) - _ptr; \
\
        /* remove the current, also to fix the linkage of \
         * the previous and the next element */ \
	int *_prev              = (_self)->hash_prev; \
	int *_next              = (_self)->next; \
        _next[_prev[_curr]]     = _next[_curr]; \
        _prev[_next[_curr]]     = _prev[_curr]; \
        /* append itself behind the icurr position */ \
        int _icurr              = (_self)->icurr; \
        _prev[_next[_icurr]]    = _curr; \
        _next[_curr]            = _next[_icurr]; \
        _next[_icurr]           = _curr; \
        _prev[_curr]            = _icurr; \
}


#endif // HASHMAP_H_INCLUDED
