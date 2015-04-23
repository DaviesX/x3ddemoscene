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


static inline void create_alg_llist(struct alg_llist* llist, int elm_size, int init_count);
static inline void free_alg_llist(struct alg_llist* llist);
PseudoDef(void alg_llist_i(struct alg_llist* llist, int i, void** elm);)
PseudoDef(void alg_llist_add(void* elm, struct alg_llist* llist);)
PseudoDef(void alg_llist_push(void* elm, struct alg_llist* llist);)
PseudoDef(void alg_llist_pop(struct alg_llist* llist, void** elm ); )
PseudoDef(void alg_llist_find(struct alg_llist* llist, void* info, void** elm_data,
                               bool (*compare_func) (void* info, void* elm));)
PseudoDef(void alg_llist_remove(struct alg_llist* llist, void* info, void** elm_data,
                                 bool (*compare_func) (void* info, void* elm));)
PseudoDef(void alg_llist_remove_ptr(void* elm, struct alg_llist* llist);)
static inline void  alg_llist_flush(struct alg_llist* list);



static inline void create_alg_llist(struct alg_llist* llist, int elm_size, int init_count)
{
        memset ( llist, 0, sizeof *llist );
        llist->content = (int*) alloc_var ( elm_size, 1 );
        llist->prev = (int*) alloc_var ( sizeof (int), 2 );
        llist->next = (int*) alloc_var ( sizeof (int), 2 );
        llist->prev = (int*) expand2_var ( llist->prev, 2 );
        llist->next = (int*) expand2_var ( llist->next, 2 );
        llist->icurr = 1;
        llist->ilast = 1;
        llist->head = llist->prev;
        llist->head[0] = llist->icurr;
        llist->prev[llist->icurr] = 0;
        llist->next[llist->icurr] = -1;
        llist->num_elm = 2 + 1;
        llist->elm_size = elm_size;
}

static inline void free_alg_llist(struct alg_llist* llist)
{
        free_var ( llist->prev );
        free_var ( llist->next );
        free_var ( llist->content );
        memset ( llist, 0, sizeof *llist );
}

static inline void alg_llist_flush(struct alg_llist* llist)
{
        flush_var ( llist->content );
        flush_var ( llist->next );
        flush_var ( llist->prev );
        llist->icurr = 1;
        llist->ilast = 1;
        llist->head[0] = llist->icurr;
        llist->num_elm = 1;
}

#define alg_llist_inject( _data, _iter, _inst ) \
{ \
        (_inst)->prev = expand2_var ( (_inst)->prev, (_inst)->num_elm ); \
        (_inst)->head = (_inst)->prev; \
        (_inst)->next = expand2_var ( (_inst)->next, (_inst)->num_elm ); \
        (_inst)->content = expand2_var ( (_inst)->content, (_inst)->num_elm ); \
\
        int _i = (_inst)->icurr; \
        int _j = (_iter) - (typeof(_iter)) (_inst)->content; \
\
        typeof(_data)* _dest = (typeof(_data)*) (_inst)->content + _i; \
        *_dest = (_data);\
\
        if ( (_inst)->icurr == (_inst)->ilast ) { \
                (_inst)->icurr = ++ (_inst)->ilast; \
                (_inst)->prev[(_inst)->icurr] = (_inst)->prev[_i]; \
        } else \
                (_inst)->icurr = (_inst)->next[_i]; \
        (_inst)->next[(_inst)->prev[_i]] = (_inst)->icurr; \
\
        (_inst)->icurr = (_inst)->next[(_inst)->icurr]; \
        (_inst)->next[(_inst)->prev[_j]] = _i; \
        (_inst)->prev[_i] = (_inst)->prev[_j]; \
        (_inst)->next[_i] = _j; \
        (_inst)->prev[_j] = _i; \
        (_inst)->num_elm ++; \
}

#define alg_llist_push_back( _data, _inst ) \
{ \
        (_inst)->prev = expand2_var ( (_inst)->prev, (_inst)->num_elm ); \
        (_inst)->head = (_inst)->prev; \
        (_inst)->next = expand2_var ( (_inst)->next, (_inst)->num_elm ); \
        (_inst)->content = expand2_var ( (_inst)->content, (_inst)->num_elm ); \
\
        int _i = (_inst)->icurr; \
        typeof(_data)* _dest = (typeof(_data)*) (_inst)->content + _i; \
        *(_dest) = (_data); \
\
        if ( (_inst)->icurr == (_inst)->ilast ) { \
                (_inst)->next[_i] = ++ (_inst)->ilast; \
                (_inst)->prev[(_inst)->next[_i]] = _i; \
        } \
        (_inst)->icurr = (_inst)->next[_i]; \
        (_inst)->num_elm ++; \
}

#define alg_llist_push_front( _data, _inst ) \
{ \
        (_inst)->prev = expand2_var ( (_inst)->prev, (_inst)->num_elm ); \
        (_inst)->head = (_inst)->prev; \
        (_inst)->next = expand2_var ( (_inst)->next, (_inst)->num_elm ); \
        (_inst)->content = expand2_var ( (_inst)->content, (_inst)->num_elm ); \
\
        int _i = (_inst)->icurr; \
        typeof(_data)* _dest = (typeof(_data)*) (_inst)->content + _i; \
        *_dest = (_data);\
\
        if ( (_inst)->icurr == (_inst)->ilast ) { \
                (_inst)->icurr = ++ (_inst)->ilast; \
                (_inst)->prev[(_inst)->icurr] = (_inst)->prev[_i]; \
        } else \
                (_inst)->icurr = (_inst)->next[_i]; \
        (_inst)->next[(_inst)->prev[_i]] = (_inst)->icurr; \
\
        (_inst)->prev[(_inst)->head[0]] = _i; \
        (_inst)->prev[_i] = -1; \
        (_inst)->next[_i] = (_inst)->head[0]; \
        (_inst)->head[0] = _i; \
        (_inst)->num_elm ++; \
}

#define alg_llist_insert( _data, _iter, _cmp, _inst ) \
{ \
        (_iter) = nullptr; \
        int _curr = (_inst)->head[0]; \
        typeof(_iter) _ptr = (_inst)->content; \
        while ( _curr != (_inst)->icurr ) { \
                if ( _cmp ( _data, &_ptr[_curr] ) ) { \
                        (_iter) = &_ptr[_curr]; \
                        alg_llist_inject ( _data, _iter, _inst ); \
                        break; \
                } \
                _curr = (_inst)->next[_curr]; \
        } \
}

#define alg_llist_find( _data, _iter, _cmp, _inst ) \
{ \
        (_iter) = nullptr; \
        int _curr = (_inst)->head[0]; \
        typeof(_iter) _ptr = (_inst)->content; \
        while ( _curr != (_inst)->icurr ) { \
                if ( _cmp ( _data, &_ptr[_curr] ) ) { \
                        (_iter) = &_ptr[_curr]; \
                        break; \
                } \
                _curr = (_inst)->next[_curr]; \
        } \
}

#define alg_llist_remove( _iter, _inst ) \
{ \
        typeof(_iter) _ptr = (_inst)->content; \
        int _i = (_iter) - _ptr; \
        (_inst)->prev[(_inst)->next[(_i)]] = (_inst)->prev[(_i)]; \
        (_inst)->next[(_inst)->prev[(_i)]] = (_inst)->next[(_i)]; \
        if ( (_inst)->icurr != (_inst)->ilast ) \
                (_inst)->prev[(_inst)->next[(_inst)->icurr]] = (_i); \
        (_inst)->next[(_inst)->icurr] = (_i); \
        (_inst)->num_elm --; \
}

#define alg_llist_pop_front( _iter, _inst ) \
{ \
        if ( (_inst)->icurr != (_inst)->head[0] ) { \
                int _front = (_inst)->head[0]; \
                typeof(_iter) _ptr = (_inst)->content; \
                alg_llist_remove ( &_ptr[_front], _inst ); \
        } \
}

#define alg_llist_pop_back( _iter, _inst ) \
{ \
        if ( (_inst)->icurr != (_inst)->head[0] ) { \
                int _back = (_inst)->prev[(_inst)->icurr]; \
                typeof(_iter) _ptr = (_inst)->content; \
                alg_llist_remove ( &_ptr[_back], _inst ); \
        } \
}

#define alg_llist_n( _inst )             ((_inst)->num_elm)

#define alg_llist_first( _iter, _inst ) \
{ \
        typeof(_iter) _ptr = (_inst)->content; \
        int _i = (_inst)->head[0]; \
        _iter = &_ptr[_i]; \
}

#define alg_llist_next( _iter, _inst ) \
{ \
        typeof(_iter) _ptr = (_inst)->content; \
        int _i = (_inst)->next[(_iter) - _ptr]; \
        _iter = &_ptr[_i]; \
}

#define alg_llist_prev( _iter, _inst ) \
{ \
        typeof(_iter) _ptr = (_inst)->content; \
        int _i = (_inst)->prev[(_iter) - _ptr]; \
        _iter = &_ptr[i]; \
}

#define alg_llist_last( _iter, _inst ) \
{ \
        typeof(_iter) _ptr = (_inst)->content; \
        int _i = (_inst)->prev[(_inst)->icurr]; \
        _iter = &_ptr[_i]; \
}

#define alg_llist_null( _iter, _inst ) \
{ \
        typeof(_iter) _ptr = (_inst)->content; \
        (_iter) = &_ptr[(_inst)->icurr]; \
}


#endif // DOUBLELINKEDLIST_H_INCLUDED
