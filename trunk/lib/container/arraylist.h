#ifndef ARRAYLIST_H_INCLUDED
#define ARRAYLIST_H_INCLUDED


#include <container/container.h>

#pragma pack(4)

/* Sometimes we need to put the pointers of
 * an array of data into a separate self for non-linear
 * access or arrangment temporarily or perminently. When the
 * process is done, the alg_pointer_list can restore the lineary order
 * with a new copy of array, while alg_double_pointer_list does not
 * both restoring process are O(n) */
struct alg_ptr_list {
        void *ptr;
};

/* Provides a self container and
 * some common O(n) operations below */
struct alg_list {
        void*           content;
        int             elm_size;
        int             num_elm;
};

#pragma pack()

PseudoDef(void alg_ptr_list_init(struct alg_ptr_list* self, void* list, int len, bool is_inplace);)
PseudoDef(void alg_ptr_list_free(struct alg_ptr_list* self);)
PseudoDef(void alg_ptr_list_tonew(struct alg_ptr_list* self, int len, void* new_list))
PseudoDef(void alg_ptr_list_inplace(struct alg_ptr_list* self, int len, void* list))

static inline void alg_list_init(struct alg_list* self, int elm_size, int init_count);
static inline void alg_list_free(struct alg_list* self);
static inline void alg_list_push_back(struct alg_list* self, void* elm);
static inline void alg_list_expand(struct alg_list* self, int count);
static inline void alg_list_flush(struct alg_list* self);
PseudoDef(void alg_list_find(struct alg_list* self, void* info, void** elm_data,
                             bool (*compare_func) (void* info, void* elm));)
PseudoDef(void alg_list_remove(struct alg_list* self, void* info,
                               bool (*compare_func) (void* info, void* elm));)
PseudoDef(int alg_list_n(struct alg_list* self);)
static inline void alg_list_copy(struct alg_list* list0, struct alg_list* list1);
static inline void alg_list_swap(struct alg_list* list0, struct alg_list* list1);



static inline void alg_list_init(struct alg_list* self, int elm_size, int init_count)
{
        zero_obj(self);
        if (init_count < 0) {
                self->content = alloc_var(elm_size, 0);
        } else {
                self->content = alloc_var(elm_size, init_count);
        }
        self->elm_size  = elm_size;
        self->num_elm   = 0;
}

static inline void alg_list_free(struct alg_list* self)
{
        free_var(self->content);
        zero_obj(self);
}

static inline void alg_list_expand(struct alg_list* self, int count)
{
        self->content = alloc_expand2_var(self->content, count);
}

static inline void alg_list_flush(struct alg_list* self)
{
        alloc_flush_var(self->content);
        self->num_elm = 0;
}

/* Both lists must have the same element type */
static inline void alg_list_copy(struct alg_list* list0, struct alg_list* list1)
{
        untyped *t      = (untyped*) list1->content;
        memcpy(list1, list0, sizeof(*list1));
        list1->content  = t;
        list1->content  = alloc_expand_var(list1->content, list0->num_elm);
        memcpy(list1->content, list0->content, list0->num_elm*list0->elm_size);
}

/* swaps the self's pointer respectively */
static inline void alg_list_swap(struct alg_list* list0, struct alg_list* list1)
{
        untyped *t      = (untyped*) list0->content;
        list0->content  = list1->content;
        list1->content  = t;
}


#define alg_list_find(_self, _data, _iter, f_Compare)	\
{\
	(_iter)                 = nullptr; \
	typeof(_iter) _ptr      = (_self)->content; \
	int _i; \
	for (_i = 0; _i < (_self)->num_elm; _i ++) { \
		if (f_Compare((_data), &_ptr[_i])) { \
			(_iter) = &_ptr[_i]; \
			break; \
		} \
	} \
}

#define alg_list_push_back(_self, _data) \
{ \
        (_self)->content        = alloc_add_var((_self)->content, sizeof(_data)); \
        typeof(_data)* _dest    = (typeof(_data)*) (_self)->content + (_self)->num_elm; \
        *_dest                  = (_data); \
        (_self)->num_elm ++; \
}

#define alg_list_pop_back(_self, _iter) \
{ \
        (_self)->content = alloc_dec_var((_self)->content, sizeof *(_iter)); \
        (_self)->num_elm --; \
}

#define alg_list_n(_self)               (_self)->num_elm
#define alg_list_array(_self)           (_self)->content
#define alg_list_first(_self, _iter)    ((_iter) = (_self)->content)
#define alg_list_next(_self, _iter)     (++ (_iter))
#define alg_list_prev(_self, _iter)     (-- (_iter))
#define alg_list_last(_self, _iter)     ((_iter) = &(cast(_iter) (_self)->content)[(_self)->num_elm - 1])
#define alg_list_null(_self, _iter)     ((_iter) = cast(_iter) (_self)->content + (_self)->num_elm)


#define alg_ptr_list_i(_self, i)	((_self)[i].ptr)

#define alg_ptr_list_init(_self, _list, _len, _is_inplace) \
{ \
        int _ptr_len    = (_len)*(_is_inplace + 1); \
        (_self)->ptr    = alloc_fix(sizeof *(_self), _ptr_len);\
        int _i; \
        for (_i = 0; _i < (_len); _i ++) { \
                (_self)[_i].ptr = &(_list)[_i]; \
        } \
}

#define alg_ptr_list_free(_self)         (free_fix(_self))

#define alg_ptr_list_tonew(_self, _len, _new_list) \
{ \
        int _i; \
        for (_i = 0; _i < (_len); _i ++) { \
                _new_list[_i] = *(cast(_new_list) (_self)[_i].ptr); \
        } \
}

#define alg_ptr_list_inplace(_self, _len, _list) \
{ \
        typeof(&_list) _ori_ptrs        = cast(&_list) &(_self)[_len]; \
        typeof(&_list) _ptrs            = cast(&_list) &(_self)[0]; \
        int _i; \
        for (_i = 0; _i < (_len); _i ++) { \
                _ori_ptrs[(_ptrs)[_i] - (_list)] = &_list[_i]; \
        } \
        typeof (*(_list))       _temp[2]; \
        _temp[0] = (_list)[0]; \
\
        int _i_iter     = 0; \
        int _i_elm      = 0; \
        int _switcher   = 0; \
        while (_i_elm < (_len)) { \
                if ((_ptrs)[_i_iter] != &(_list)[_i_iter]) { \
                        (_ptrs)[_i_iter]                = &(_list)[_i_iter]; \
                        _i_iter                         = (_ptrs)[_i_iter] - (_list); \
                        _temp[(_switcher + 1) & 1]      = (_list)[_i_iter]; \
                        (_list)[_i_iter]                = _temp[_switcher & 1]; \
                        _switcher ++; \
                } else { \
                        do { \
                                _i_iter ++; \
                                _i_elm ++; \
                        } while (&(_list)[_i_iter] == (_ptrs)[_i_iter]); \
                        _temp[_switcher & 1] = (_list)[_i_iter]; \
                } \
        } \
        free_fix(_ptrs); \
}

#endif // ARRAYLIST_H_INCLUDED
