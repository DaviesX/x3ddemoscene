#ifndef ARRAYLIST_H_INCLUDED
#define ARRAYLIST_H_INCLUDED


#include <container/container.h>

#pragma pack(4)

/* Sometimes we need to put the pointers of
 * an array of data into a separate list for non-linear
 * access or arrangment temporarily or perminently. When the
 * process is done, the alg_pointer_list can restore the lineary order
 * with a new copy of array, while alg_double_pointer_list does not
 * both restoring process are O(n) */
struct alg_ptr_list {
        void *ptr;
};

/* Provides a list container and
 * some common O(n) operations below */
struct alg_list {
        void*           content;
        int             elm_size;
        int             num_elm;
};

#pragma pack()

pseudo_def(void create_alg_ptr_list(void* list, int len, bool is_inplace, struct alg_ptr_list* ptrs);)
pseudo_def(void free_alg_ptr_list(struct alg_ptr_list* ptrs );)
pseudo_def(void alg_ptr_list_tonew(struct alg_ptr_list* ptrs, int len, void* new_list))
pseudo_def(void alg_ptr_list_inplace(struct alg_ptr_list* ptrs, int len, void* list))

static inline void create_alg_list(struct alg_list* list, int elm_size, int init_count);
static inline void free_alg_list(struct alg_list* list);
static inline void alg_list_push_back(void* elm, struct alg_list* list);
static inline void alg_list_expand(int count, struct alg_list* list);
static inline void alg_list_flush(struct alg_list* list);
pseudo_def(void alg_list_find(struct alg_list* list, void* info, void** elm_data,
                              bool (*compare_func) (void* info, void* elm));)
pseudo_def(void alg_list_remove(struct alg_list* list, void* info,
                                bool (*compare_func) (void* info, void* elm));)
pseudo_def(int alg_list_n(struct alg_list* list);)
static inline void alg_list_copy(struct alg_list* list0, struct alg_list* list1);
static inline void alg_list_swap(struct alg_list* list0, struct alg_list* list1);



static inline void create_alg_list(struct alg_list* list, int elm_size, int init_count)
{
        memset ( list, 0, sizeof ( *list ) );
        if ( init_count < 0 ) {
                list->content = alloc_var ( elm_size, 0 );
        } else {
                list->content = alloc_var ( elm_size, init_count );
        }
        list->elm_size = elm_size;
        list->num_elm = 0;
}

static inline void free_alg_list(struct alg_list* list)
{
        free_var ( list->content );
        zero_obj ( list );
}

static inline void alg_list_expand(int count, struct alg_list* list)
{
        list->content = expand2_var ( list->content, count );
}

static inline void alg_list_flush(struct alg_list* list)
{
        flush_var ( list->content );
        list->num_elm = 0;
}

/* Both lists must have the same element type */
static inline void alg_list_copy(struct alg_list* list0, struct alg_list* list1)
{
        untyped *t = (untyped*) list1->content;
        memcpy ( list1, list0, sizeof ( *list1 ) );
        list1->content = t;
        list1->content = expand_var ( list1->content, list0->num_elm );
        memcpy ( list1->content, list0->content, list0->num_elm*list0->elm_size );
}

/* swaps the list's pointer respectively */
static inline void alg_list_swap(struct alg_list* list0, struct alg_list* list1)
{
        untyped *t = (untyped*) list0->content;
        list0->content = list1->content;
        list1->content = t;
}


#define alg_list_find( _data, _iter, _cmp, _inst )	\
{\
	(_iter) = nullptr; \
	typeof(_iter) _ptr = (_inst)->content; \
	int _i; \
	for ( _i = 0; _i < (_inst)->num_elm; _i ++ ) { \
		if ( _cmp ( (_data), &_ptr[_i] ) ) { \
			(_iter) = &_ptr[_i]; \
			break; \
		} \
	} \
}

#define alg_list_push_back( _data, _inst ) \
{ \
        (_inst)->content = add_var ( (_inst)->content, sizeof (_data) ); \
        typeof(_data)* _dest = (typeof(_data)*) (_inst)->content + (_inst)->num_elm; \
        *_dest = (_data); \
        (_inst)->num_elm ++; \
}

#define alg_list_pop_back( _iter, _inst ) \
{ \
        (_inst)->content = dec_var ( (_inst)->content, sizeof *(_iter) ); \
        (_inst)->num_elm --; \
}

#define alg_list_n( _inst )             (_inst)->num_elm
#define alg_list_array( _inst )         (_inst)->content
#define alg_list_first( _iter, _inst )  ((_iter) = (_inst)->content)
#define alg_list_next( _iter, _inst )   (++ (_iter))
#define alg_list_prev( _iter, _inst )   (-- (_iter))
#define alg_list_last( _iter, _inst )   ((_iter) = &(cast(_iter) (_inst)->content)[(_inst)->num_elm - 1])
#define alg_list_null( _iter, _inst )   ((_iter) = cast(_iter) (_inst)->content + (_inst)->num_elm)


#define alg_ptr_list_i( _ptr_l, i )	((_ptr_l)[i].ptr)

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

#endif // ARRAYLIST_H_INCLUDED
