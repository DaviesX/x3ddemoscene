#ifndef STAGING_H_INCLUDED
#define STAGING_H_INCLUDED

#include <container/hashmap.h>
#include <container/arraylist.h>
#include <system/allocator.h>

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

typedef struct entry_record entry_rec_t;

enum RES_ERROR {
        RES_ERR_INVALID,
        RES_ERR_READ_BLOCKED,
        RES_ERR_NORMAL
};

/*
 * Functions declaration
 */

void create_stager ( struct mem_block *block, int init_count, struct stager *stg );
void free_stager ( struct stager *stg );
void stager_flush ( struct stager *stg );

entry_rec_t *create_entry_record ( struct stager *rp );
void free_entry_record ( entry_rec_t *rec );

int stager_begin_entry ( char *name, entry_rec_t *rec );
void stager_end_entry ( int rdepth, entry_rec_t *rec );
void stager_push_data ( void *data, int size, entry_rec_t *rec );

void stager_enter_root ( char *sname, entry_rec_t *rec );
void stager_enter_sub ( char *sname, entry_rec_t *rec );
void stager_leave_node ( entry_rec_t *rec );

enum RES_ERROR stager_set_layer_first ( entry_rec_t *rec );
enum RES_ERROR stager_set_layer_next ( entry_rec_t *rec );
enum RES_ERROR stager_reset_layer ( entry_rec_t *rec );
enum RES_ERROR stager_delete_node ( entry_rec_t *rec );
void stager_reset_data ( entry_rec_t *rec );
void *stager_pop_data ( int size, entry_rec_t *rec );

#define free_entry_record_s( _rec ) \
{ \
	free_entry_record ( _rec ); \
	(_rec) = nullptr; \
}

/* These functions are good for dealing with a variable, but not for a chunk of data */
#define stager_push_data_c( _data, _size, _rec ) \
{ \
	typeof (_data) _c = _data; \
	stager_push_data ( &_c, _size, _rec ); \
}

#define stager_pop_data_c( TYPE, _rec  ) \
	(*(TYPE *) stager_pop_data ( sizeof ( TYPE ), _rec ))


#endif // STAGING_H_INCLUDED
