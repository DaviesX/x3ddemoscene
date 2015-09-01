#ifndef DOUBLELINKEDLIST_H_INCLUDED
#define DOUBLELINKEDLIST_H_INCLUDED


#include <container/container.h>

#pragma pack(4)

/* linked list */
struct alg_llist {
        void*   content;
        int*    head;
        int*    prev;
        int*    next;
        int     ilast;
        int     icurr;
        int     elm_size;
        int     num_elm;
};

#pragma pack()


static inline void alg_llist_init(struct alg_llist* llist, int elm_size, int init_count);
static inline void alg_llist_free(struct alg_llist* self);
PseudoDef(void alg_llist_i(struct alg_llist* self, int i, void** elm);)
PseudoDef(void alg_llist_add(void* elm, struct alg_llist* self);)
PseudoDef(void alg_llist_push(void* elm, struct alg_llist* self);)
PseudoDef(void alg_llist_pop(struct alg_llist* self, void** elm ); )
PseudoDef(void alg_llist_find(struct alg_llist* self, void* info, void** elm_data,
                               bool (*compare_func) (void* info, void* elm));)
PseudoDef(void alg_llist_remove(struct alg_llist* self, void* info, void** elm_data,
                                 bool (*compare_func) (void* info, void* elm));)
PseudoDef(void alg_llist_remove_ptr(void* elm, struct alg_llist* self);)
static inline void  alg_llist_flush(struct alg_llist* list);



static inline void alg_llist_init(struct alg_llist* self, int elm_size, int init_count)
{
        memset ( self, 0, sizeof *self );
        self->content = (int*) alloc_var ( elm_size, 1 );
        self->prev = (int*) alloc_var ( sizeof (int), 2 );
        self->next = (int*) alloc_var ( sizeof (int), 2 );
        self->prev = (int*) expand2_var ( self->prev, 2 );
        self->next = (int*) expand2_var ( self->next, 2 );
        self->icurr = 1;
        self->ilast = 1;
        self->head = self->prev;
        self->head[0] = self->icurr;
        self->prev[self->icurr] = 0;
        self->next[self->icurr] = -1;
        self->num_elm = 2 + 1;
        self->elm_size = elm_size;
}

static inline void alg_llist_free(struct alg_llist* self)
{
        free_var ( self->prev );
        free_var ( self->next );
        free_var ( self->content );
        memset ( self, 0, sizeof *self );
}

static inline void alg_llist_flush(struct alg_llist* self)
{
        flush_var ( self->content );
        flush_var ( self->next );
        flush_var ( self->prev );
        self->icurr = 1;
        self->ilast = 1;
        self->head[0] = self->icurr;
        self->num_elm = 1;
}

#define alg_llist_inject(_self, _data, _iter) \
{ \
        (_self)->prev           = expand2_var((_self)->prev, (_self)->num_elm); \
        (_self)->head           = (_self)->prev; \
        (_self)->next           = expand2_var((_self)->next, (_self)->num_elm); \
        (_self)->content        = expand2_var((_self)->content, (_self)->num_elm); \
\
        int _i = (_self)->icurr; \
        int _j = (_iter) - (typeof(_iter)) (_self)->content; \
\
        typeof(_data)* _dest = (typeof(_data)*) (_self)->content + _i; \
        *_dest = (_data);\
\
        if ((_self)->icurr == (_self)->ilast) { \
                (_self)->icurr                = ++ (_self)->ilast; \
                (_self)->prev[(_self)->icurr] = (_self)->prev[_i]; \
        } else \
                (_self)->icurr                = (_self)->next[_i]; \
\
        (_self)->next[(_self)->prev[_i]] = (_self)->icurr; \
        (_self)->icurr                   = (_self)->next[(_self)->icurr]; \
        (_self)->next[(_self)->prev[_j]] = _i; \
        (_self)->prev[_i]                = (_self)->prev[_j]; \
        (_self)->next[_i]                = _j; \
        (_self)->prev[_j]                = _i; \
        (_self)->num_elm ++; \
}

#define alg_llist_push_back(_self, _data) \
{ \
        (_self)->prev           = expand2_var((_self)->prev, (_self)->num_elm); \
        (_self)->head           = (_self)->prev; \
        (_self)->next           = expand2_var((_self)->next, (_self)->num_elm); \
        (_self)->content        = expand2_var((_self)->content, (_self)->num_elm); \
\
        int _i               = (_self)->icurr; \
        typeof(_data)* _dest = (typeof(_data)*) (_self)->content + _i; \
        *(_dest)             = (_data); \
\
        if ((_self)->icurr == (_self)->ilast) { \
                (_self)->next[_i]                = ++ (_self)->ilast; \
                (_self)->prev[(_self)->next[_i]] = _i; \
        } \
        (_self)->icurr = (_self)->next[_i]; \
        (_self)->num_elm ++; \
}

#define alg_llist_push_front(_self, _data) \
{ \
        (_self)->prev           = expand2_var((_self)->prev, (_self)->num_elm); \
        (_self)->head           = (_self)->prev; \
        (_self)->next           = expand2_var((_self)->next, (_self)->num_elm); \
        (_self)->content        = expand2_var((_self)->content, (_self)->num_elm); \
\
        int _i               = (_self)->icurr; \
        typeof(_data)* _dest = (typeof(_data)*) (_self)->content + _i; \
        *_dest               = (_data);\
\
        if ((_self)->icurr == (_self)->ilast) { \
                (_self)->icurr                = ++ (_self)->ilast; \
                (_self)->prev[(_self)->icurr] = (_self)->prev[_i]; \
        } else \
                (_self)->icurr                = (_self)->next[_i]; \
\
        (_self)->next[(_self)->prev[_i]] = (_self)->icurr; \
        (_self)->prev[(_self)->head[0]]  = _i; \
        (_self)->prev[_i]                = -1; \
        (_self)->next[_i]                = (_self)->head[0]; \
        (_self)->head[0]                 = _i; \
        (_self)->num_elm ++; \
}

#define alg_llist_insert(_self, _data, _iter, f_Compare) \
{ \
        (_iter)                 = nullptr; \
        int _curr               = (_self)->head[0]; \
        typeof(_iter) _ptr      = (_self)->content; \
        while (_curr != (_self)->icurr) { \
                if (f_Compare(_data, &_ptr[_curr])) { \
                        (_iter) = &_ptr[_curr]; \
                        alg_llist_inject(_self, _data, _iter); \
                        break; \
                } \
                _curr = (_self)->next[_curr]; \
        } \
}

#define alg_llist_find(_self, _data, _iter, f_Compare) \
{ \
        (_iter)                 = nullptr; \
        int _curr               = (_self)->head[0]; \
        typeof(_iter) _ptr      = (_self)->content; \
        while (_curr != (_self)->icurr) { \
                if (f_Compare(_data, &_ptr[_curr])) { \
                        (_iter) = &_ptr[_curr]; \
                        break; \
                } \
                _curr = (_self)->next[_curr]; \
        } \
}

#define alg_llist_remove(_self, _iter) \
{ \
        typeof(_iter) _ptr      = (_self)->content; \
        int _i                  = (_iter) - _ptr; \
\
        (_self)->prev[(_self)->next[(_i)]]                   = (_self)->prev[(_i)]; \
        (_self)->next[(_self)->prev[(_i)]]                   = (_self)->next[(_i)]; \
        if ((_self)->icurr != (_self)->ilast) \
                (_self)->prev[(_self)->next[(_self)->icurr]] = (_i); \
        (_self)->next[(_self)->icurr]                        = (_i); \
\
        (_self)->num_elm --; \
}

#define alg_llist_pop_front(_self, _iter) \
{ \
        if ((_self)->icurr != (_self)->head[0]) { \
                int _front              = (_self)->head[0]; \
                typeof(_iter) _ptr      = (_self)->content; \
                alg_llist_remove(_self, &_ptr[_front]); \
        } \
}

#define alg_llist_pop_back(_self, _iter) \
{ \
        if ((_self)->icurr != (_self)->head[0]) { \
                int _back               = (_self)->prev[(_self)->icurr]; \
                typeof(_iter) _ptr      = (_self)->content; \
                alg_llist_remove(_self, &_ptr[_back]); \
        } \
}

#define alg_llist_n(_self)              ((_self)->num_elm)

#define alg_llist_first(_self, _iter) \
{ \
        typeof(_iter) _ptr      = (_self)->content; \
        int _i                  = (_self)->head[0]; \
        _iter                   = &_ptr[_i]; \
}

#define alg_llist_next(_self, _iter) \
{ \
        typeof(_iter) _ptr      = (_self)->content; \
        int _i                  = (_self)->next[(_iter) - _ptr]; \
        _iter                   = &_ptr[_i]; \
}

#define alg_llist_prev(_self, _iter) \
{ \
        typeof(_iter) _ptr      = (_self)->content; \
        int _i                  = (_self)->prev[(_iter) - _ptr]; \
        _iter                   = &_ptr[i]; \
}

#define alg_llist_last(_self, _iter) \
{ \
        typeof(_iter) _ptr      = (_self)->content; \
        int _i                  = (_self)->prev[(_self)->icurr]; \
        _iter                   = &_ptr[_i]; \
}

#define alg_llist_null(_self, _iter) \
{ \
        typeof(_iter) _ptr      = (_self)->content; \
        (_iter)                 = &_ptr[(_self)->icurr]; \
}


#endif // DOUBLELINKEDLIST_H_INCLUDED
