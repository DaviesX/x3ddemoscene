#ifndef STAGING_H_INCLUDED
#define STAGING_H_INCLUDED

#include <algorithm.h>

struct alg_hash_link_list;
struct alg_list;

/*
 * Structures
 */

/* Defines a structure for resource staging */
struct stager {
        int state;
        struct alg_hash_llist lookup;
        struct alg_list entry;
        int byte_used;
        int num_entry;
};

typedef struct res_record res_record_t;

enum RES_ERROR {
        RES_ERR_INVALID,
        RES_ERR_READ_BLOCKED,
        RES_ERR_NORMAL
};

/*
 * Functions declaration
 */

void init_stager ( struct stager *stg );
void free_stager ( struct stager *stg );
void flush_stager ( struct stager *stg );

res_record_t *create_res_record ( struct stager *rp );
void free_res_record ( res_record_t *rec );

int begin_entry_stager ( char *name, res_record_t *rec );
void end_entry_stager ( int rdepth, res_record_t *rec );
void push_data_stager ( void *data, int size, res_record_t *rec );

void enter_root_stager ( char *sname, res_record_t *rec );
void enter_sub_stager ( char *sname, res_record_t *rec );
void leave_node_stager ( res_record_t *rec );

enum RES_ERROR set_layer_first_stager ( struct res_record *rec );
enum RES_ERROR set_layer_next_stager ( struct res_record *rec );
enum RES_ERROR reset_layer_stager ( struct res_record *rec );
enum RES_ERROR delete_node_stager ( struct res_record *rec );
void reset_data_stager ( struct res_record *rec );
void *pop_data_stager ( int size, res_record_t *rec );

#define free_res_record_s( _rec ) \
{ \
	free_res_record ( _rec ); \
	(_rec) = nullptr; \
}

/* These functions are good for dealing with a variable, but not for a chunk of data */
#define push_data_stager_c( _data, _size, _rec ) \
{ \
	typeof (_data) _c = _data; \
	push_data_stager ( &_c, _size, _rec ); \
}

#define pop_data_stager_c( TYPE, _rec  ) \
	(*(TYPE *) pop_data_stager ( sizeof ( TYPE ), _rec ))


#endif // STAGING_H_INCLUDED
