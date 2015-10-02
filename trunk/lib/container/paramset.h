#ifndef PARAMSET_H_INCLUDED
#define PARAMSET_H_INCLUDED

#include <container/container.h>
#include <container/linkedlist.h>

struct alg_var_set {
        d_alg_llist(alg_var)       var_set;
};

/* A small parameters lookup table */
struct alg_named_params {
        int             pop_state;
        char**          str;
        int*            ref;
        int             n_ref;
        int             size;
        untyped*        buff;
        int             curr_pos;
};


void alg_var_set_init(struct alg_var_set* set);
void alg_var_set_free(struct alg_var_set* set);
void alg_var_set_declare(char* name, void* data, int size, struct alg_var_set* set);
void alg_var_set_undeclare(char* name, struct alg_var_set* set);
void* alg_var_set_use(char* name, struct alg_var_set* set);


#define init_named_params(_p) \
{ \
        memset ( _p, 0, sizeof ( *(_p) ) ); \
        (_p)->str = alloc_var ( sizeof(char*), 0 ); \
        (_p)->ref = alloc_var ( sizeof(int), 0 ); \
        (_p)->buff = alloc_var ( sizeof(untyped), 0 ); \
}

#define push_named_params(_data, _name, _p) \
{ \
        typeof (_data) _d = _data; \
\
        /* Check if I've got new stuffs */ \
        int _i = -1; \
        int eq = 1; \
        while ( ++ _i < (_p)->n_ref && (eq = strcmp ( _name, (_p)->str[_i] )) ); \
        if ( eq != 0 ) { \
                /* It does have new stuffs */ \
                (_p)->str = alloc_add_var ( (_p)->str, 1 ); \
                (_p)->str[(_p)->n_ref] = alloc_fix ( 1, strlen ( _name ) + 1 ); \
                strcpy ( (_p)->str[(_p)->n_ref], _name ); \
\
                (_p)->ref = alloc_add_var ( (_p)->ref, 1 ); \
                (_p)->ref[(_p)->n_ref] = (_p)->size; \
                (_p)->n_ref ++; \
\
                (_p)->buff = alloc_add_var ( (_p)->buff, sizeof (_d) ); \
                memcpy ( &(_p)->buff[(_p)->size], &(_d), sizeof (_d) ); \
                (_p)->size += sizeof (_d); \
        } else { \
                /* No, I'd better modify the old one */ \
                memcpy ( &(_p)->buff[(_p)->ref[_i]], &(_d), sizeof (_d) ); \
        } \
}

#define push_params(_data, _p) \
{ \
        typeof (_data) _d = _data; \
        (_p)->buff = alloc_add_var ( (_p)->buff, sizeof (_d) ); \
        memcpy ( &(_p)->buff[(_p)->size], &(_d), sizeof (_d) ); \
        (_p)->size += sizeof (_d); \
}

#define pop_named_params(_p, _name, _data) \
do { \
        int _i = -1; \
        int eq = 1; \
        while ( ++ _i < (_p)->n_ref && (eq = strcmp ( _name, (_p)->str[_i] )) ); \
        if ( eq != 0 ) { \
                /* No, I've found nothing at all */\
                (_p)->pop_state = 0; \
                break; \
        } else { \
                (_data) = *(typeof (_data) *) &(_p)->buff[(_p)->ref[_i]]; \
                (_p)->curr_pos = (_p)->ref[_i] + sizeof (_data); \
                (_p)->pop_state = 1; \
        } \
} while ( 0 )

#define pop_params(_p, _data) \
{ \
        (_data) = *(typeof (_data) *) &(_p)->buff[(_p)->curr_pos]; \
        (_p)->curr_pos += sizeof (_data); \
}

#define reset_named_params(_p) \
{ \
        int _i; \
        for ( _i = 0; _i < (_p)->n_ref; _i ++ ) { \
                free_fix ( (_p)->str[_i] ); \
                (_p)->str[_i] = nullptr; \
        } \
        alloc_flush_var ( (_p)->str ); \
        alloc_flush_var ( (_p)->ref ); \
        alloc_flush_var ( (_p)->buff ); \
        (_p)->n_ref = 0; \
        (_p)->size = 0; \
        (_p)->curr_pos = 0; \
}

#define check_named_params(_p)	        ((_p)->pop_state != 0)

#define free_named_params(_p) \
{ \
        int _i; \
        for ( _i = 0; _i < (_p)->n_ref; _i ++ ) { \
                free_fix ( (_p)->str[_i] ); \
                (_p)->str[_i] = nullptr; \
        } \
        free_var ( (_p)->str ); \
        free_var ( (_p)->ref ); \
        free_var ( (_p)->buff ); \
        memset ( (_p), 0, sizeof ( *(_p) ) ); \
}


#endif // PARAMSET_H_INCLUDED
