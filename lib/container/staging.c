/* staging.cpp: data resource staging operations go here */
#include <misc.h>
#include <container/stack.h>
#include <container/hashmap.h>
#include <container/linkedlist.h>
#include <container/staging.h>


#define HASH_SIZE		(1 << 16)
#define INIT_COUNT		(1 << 10)
#define HASH_ENTRY_RATIO	(1.0f/10.0f)

#define ENTRY_LOCKED			1
#define ENTRY_UNLOCKED			2

#define MAX_DEPTH			32

#define RESOURCE_INVALID		-1
#define RESOURCE_LOADING		-2
#define RESOURCE_LOADED			-3

struct res_name {
        int n;
        char *layer[MAX_DEPTH];
};

/* An entry of a batch of resource */
struct res_entry {
        uint32_t name_hash;
        char *name;
        uint16_t uid;
        int16_t state;
        untyped *buff;
};

/* A record of found resource entry */
struct entry_record {
        struct stager *stg;
        int rdepth;			/* recursive depth */
        struct res_name layer;
        uint32_t layer_hash;
        int ientry;			/* linked-list offset */
        uint16_t uid;
        lstack_templ(128) rec_stack;
        untyped *byte_ori;		/* memory zone of the dest entry, for _READ-ONLY_ */
        untyped *byte_curr;
        untyped *byte_end;
};

/* Packed info for hashing */
struct entry_info {
        uint32_t name_hash;
        char *name;
        uint16_t uid;
        struct res_entry *entry;
        int ifound;
};

/* predicate for hash lookup */
#define cmp_name( _info, _current ) \
	(!(((_info)->name_hash ^ (_current)->name_hash) | \
	(strcmp ( (_info)->name, (_current)->name ))))

#define cmp_uid( _info, _current )	\
	((_info)->entry[_current].uid == (_info)->uid)

#define cmp_uid_name( _info, _current ) \
	(!(((_info)->name_hash ^ (_current)->name_hash) | \
	((_current)->uid ^ (_info)->uid) | \
	(strcmp ( (_info)->name, (_current)->name ))))


uint16_t alloc_uid ( void );
void push_res_name ( char *sname, struct res_name *name );
char *pop_res_name ( struct res_name *name );
void reset_res_name ( struct res_name *name );
char *gen_layer_name ( struct res_name *name );
uint32_t gen_layer_name_hash ( char *name );
uint32_t gen_layer_hash ( struct res_name *name );
uint32_t gen_root_hash ( struct res_name *name );
char *root_layer ( struct res_name *name, int *rdepth );
void optimize_res ( struct stager *stg );

uint16_t g_uid_count = 0;


uint16_t alloc_uid ( void )
{
        assert ( g_uid_count < (1 << 16) );
        uint16_t t = g_uid_count;
        g_uid_count += MAX_DEPTH;
        return t;
}

void push_res_name ( char *sname, struct res_name *name )
{
        assert ( sname && *sname != 0 );
        name->layer[name->n] = alloc_fix ( strlen ( sname ) + 1, 1 );
        /* Revert the order and copy */
        int i, j;
        int n = strlen ( sname );
        for ( i = n - 1, j = 0; i >= 0; i --, j ++ ) {
                name->layer[name->n][j] = sname[i];
        }
        name->layer[name->n][j] = 0;
        name->n ++;
}

char *pop_res_name ( struct res_name *name )
{
        assert ( name->n > 0 );
        name->n --;
        free_fix ( name->layer[name->n] );
        return name->layer[name->n - 1];
}

void reset_res_name ( struct res_name *name )
{
        int i;
        for ( i = name->n - 1; i >= 0; i -- ) {
                pop_res_name ( name );
        }
}

char *gen_layer_name ( struct res_name *name )
{
        int l[MAX_DEPTH];
        int total = 0;
        int i;
        for ( i = 0; i < name->n; i ++ ) {
                l[i] = strlen ( name->layer[i] );
                total += l[i];
        }
        char *slayer = alloc_fix ( sizeof ( char ), total + 1 );
        /* generate in reversed order */
        int j, k;
        for ( i = 0, k = 0; i < name->n; i ++ ) {
                for ( j = 0; j < l[i]; j ++, k ++ ) {
                        slayer[k] = name->layer[i][j];
                }
        }
        slayer[total] = 0;
        return slayer;
}

uint32_t gen_layer_name_hash ( char *name )
{
        return alg_hash_str4 ( name );
}

uint32_t gen_layer_hash ( struct res_name *name )
{
        uint32_t hash = 0;
        typedef uint32_t (*hash_func) (const char *str);
        const hash_func str_hash[4] = {
                alg_hash_str0,
                alg_hash_str1,
                alg_hash_str2,
                alg_hash_str3
        };
        int i;
        int i_func = 0;
        for ( i = 0; i < name->n; i ++ ) {
                hash += str_hash[i_func ++ & 3] ( name->layer[i] );
        }
        return hash;
}

uint32_t gen_root_hash ( struct res_name *name )
{
        assert ( name->layer[0] );
        return alg_hash_str0 ( name->layer[0] );
}

/* extract root name and layer depth */
char *root_layer ( struct res_name *name, int *rdepth )
{
        assert ( name->n > 0 );
        *rdepth = name->n - 1;
        return name->layer[0];
}

void create_stager ( struct arena_allocator *block, int init_count, struct stager *stg )
{
        init_count = init_count > 0 ? init_count : INIT_COUNT;
        int hash_size = init_count*4;
        memset ( stg, 0, sizeof ( *stg ) );
        alg_hash_llist_init(&stg->lookup, hash_size, init_count, sizeof (struct res_entry));
        alg_list_init(&stg->entry, sizeof ( struct res_entry ), INIT_COUNT);
}

void free_stager ( struct stager *stg )
{
        alg_free(hash_llist, &stg->lookup);
        struct res_entry* entries = alg_array(list, &stg->entry);
        int i;
        for ( i = 0; i < alg_n ( llist, &stg->entry ); i ++ ) {
                struct res_entry* e = &entries[i];
                free_fix ( e->name );
                free_var ( e->buff );
        }
        alg_list_free ( &stg->entry );
        memset ( stg, 0, sizeof ( *stg ) );
}

void stager_flush ( struct stager *stg )
{
        alg_hash_llist_flush ( &stg->lookup );
        struct res_entry* entries = alg_array(list, &stg->entry);
        int i;
        for ( i = 0; i < alg_list_n ( &stg->entry ); i ++ ) {
                struct res_entry *e = &entries[i];
                free_fix ( e->name );
                free_var ( e->buff );
        }
        alg_list_flush ( &stg->entry );
        stg->byte_used = 0;
}

/* This function will be called each time a new entry is created
 * It checks and kills obselete nodes, and rehash the hash table
 * whenever the amount of entry is rising over the threshold */
void optimize_res ( struct stager *stg )
{
}

/* Create a new entry to write in data, this function must pair up with the
 * end_entry_resource_piler() once the data segment has been all pushed in
 * the return value is the current rc_count of hierachy*/
int stager_begin_entry ( char *name, struct entry_record *rec )
{
        struct stager *stg = rec->stg;
        optimize_res ( stg );

        push_res_name ( name, &rec->layer );
        int d;
        char *lroot = root_layer ( &rec->layer, &d );
        uint16_t layer_uid;
        uint16_t root_uid;

        if ( rec->rdepth == 0 ) {
                /* Root must be _UNIQUE_ */
                struct entry_info root_info = {
                        .entry = alg_array ( list, &stg->entry ),
                        .name = lroot,
                        .name_hash = gen_layer_name_hash ( lroot ),
                        .ifound = NIL_POSITION
                };
                DebugSession (
                        assert ( d == 0 );
                        int hash = gen_layer_hash ( &rec->layer );
                        struct res_entry *entry;
                        alg_hash_llist_first ( &stg->lookup, hash, &entry, &root_info, cmp_name );
                        assert ( entry != nullptr );
                );
                root_uid = alloc_uid ();
        } else {
                /* Retrieve the root uid from last call */
                root_uid = rec->uid;
        }
        layer_uid = root_uid + d;

        /* Set up entry and add it to the entry list */
        struct res_entry entry = {0};
        entry.state = RESOURCE_LOADING;
        entry.name = gen_layer_name ( &rec->layer );
        entry.name_hash = gen_layer_name_hash ( entry.name );
        entry.buff = alloc_var ( sizeof ( untyped ), 0 );
        entry.uid = layer_uid;
        int i = alg_list_n ( &stg->entry );
        alg_push_back ( list, &stg->entry, entry );
        stg->num_entry ++;

        /* Insert the current position to the hash linked list */
        int hash = gen_layer_hash ( &rec->layer );
        alg_hash_llist_add ( &stg->lookup, &entry, hash, f_Dym_Update );

        rec->uid = root_uid;
        rec->ientry = i;
        lstack_push ( &rec->rec_stack, i );
        assert ( rec->rdepth >= 0 );
        rec->rdepth ++;
        return rec->rdepth;
}

/* End a data segment, you must pass a correct rdepth the same as
 * the one you received from begin_entry_resource_piler */
void stager_end_entry ( int rdepth, struct entry_record *rec )
{
        assert ( rec->rdepth == rdepth );
        rec->rdepth --;
        assert ( rec->rdepth >= 0 );
        struct stager *stg = rec->stg;

        int ientry;
        lstack_pop ( &rec->rec_stack, ientry );
        struct res_entry* entries = alg_array ( list, &stg->entry );
        struct res_entry* entry = &entries[ientry];
        entry->state = RESOURCE_LOADED;
        pop_res_name ( &rec->layer );
        uint16_t root_uid = rec->uid;
        DebugSession (
                assert ( entry->uid == root_uid + rec->rdepth );
        )
}

/* Push data to the current segment created by begin_entry_resource_piler () */
void stager_push_data ( void *data, int size, struct entry_record *rec )
{
        assert ( rec->rdepth > 0 );
        struct stager *stg = rec->stg;
        struct res_entry* entries = alg_array ( list, &stg->entry );
        struct res_entry* entry = &entries[rec->ientry];
        entry->buff = alloc_var_add ( entry->buff, size );
        untyped *curr_byte = alloc_var_get_last ( entry->buff ) - (size - 1);
        memcpy ( curr_byte, data, size );
        stg->byte_used += size;
}

/* entry_record is used to read/write the resource piler it binds with */
struct entry_record *create_entry_record ( struct stager *stg )
{
        struct entry_record *rec = alloc_fix ( sizeof ( *rec ), 1 );
        memset ( rec, 0, sizeof ( *rec ) );
        lstack_init ( &rec->rec_stack );
        rec->stg = stg;
        return rec;
}

void free_entry_record ( struct entry_record *rec )
{
        reset_res_name ( &rec->layer );
        free_fix ( rec );
}

/* These functions change only the state, stager_set_layer_first()
 * Takes true action to search for resource entry */
void stager_enter_root ( char *sname, struct entry_record *rec )
{
        reset_res_name ( &rec->layer );
        push_res_name ( sname, &rec->layer );
}

void stager_enter_sub ( char *sname, struct entry_record *rec )
{
        push_res_name ( sname, &rec->layer );
}

void stager_leave_node ( struct entry_record *rec )
{
        pop_res_name ( &rec->layer );
}

/* Search for the right entry of the right layer */
enum RES_ERROR stager_set_layer_first ( struct entry_record *rec )
{
        struct stager *stg = rec->stg;
        int d;
        char *sroot = root_layer ( &rec->layer, &d );
        char *slayer = gen_layer_name ( &rec->layer );

        /* Search the root for uid */
        struct entry_info info = {
                .entry = alg_array ( list, &stg->entry ),
                .name = sroot,
                .name_hash = gen_layer_name_hash ( sroot ),
                .ifound = NIL_POSITION
        };
        uint32_t hash = gen_root_hash ( &rec->layer );
        struct res_entry* entry;
        alg_hash_llist_first ( &stg->lookup, hash, &entry, &info, cmp_name );
        if ( entry == nullptr ) {
                return RES_ERR_INVALID;
        }
        struct res_entry* entries = alg_array(list, &stg->entry);
        entry = &entries[info.ifound];
        uint16_t root_uid = entry->uid;
        uint16_t layer_uid = root_uid + d;

        /* Search the layer if needed to */
        if ( d > 0 ) {
                info.entry = alg_array(list, &stg->entry);
                info.name = slayer;
                info.name_hash = gen_layer_name_hash ( slayer );
                info.ifound = NIL_POSITION;
                info.uid = layer_uid;

                hash = gen_layer_hash ( &rec->layer );
                alg_hash_llist_first ( &stg->lookup, hash, &entry, &info, cmp_uid_name );
                if ( entry == nullptr ) {
                        free_fix ( slayer );
                        return RES_ERR_INVALID;
                }
        }
        rec->ientry = info.ifound;
        free_fix ( slayer );
        /* Retrieve the entry */
        entry = &entries[info.ifound];
        assert ( entry->state != RESOURCE_INVALID );
        if ( entry->state == RESOURCE_LOADING ) {
                return RES_ERR_READ_BLOCKED;
        }

        rec->layer_hash = hash;
        rec->uid = root_uid;
        rec->rdepth = d;
        rec->byte_ori = entry->buff;
        rec->byte_curr = entry->buff;
        rec->byte_end = alloc_var_get_last ( entry->buff );
        return RES_ERR_NORMAL;
}

/* Search for next entry of the same layer */
enum RES_ERROR stager_set_layer_next ( struct entry_record *rec )
{
        struct stager *stg = rec->stg;
        int d;
        /*char *sroot = */ root_layer ( &rec->layer, &d );
        char *slayer = gen_layer_name ( &rec->layer );
        struct entry_info info = {
                .entry = alg_array(list, &stg->entry),
                .name = slayer,
                .name_hash = gen_layer_name_hash ( slayer ),
                .ifound = rec->ientry,
                .uid = rec->uid + d
        };
        /* Retrieve the entry */
        struct res_entry *entry;
        alg_hash_llist_next ( &stg->lookup, &entry, &info, cmp_uid_name );
        free_fix ( slayer );
        if ( entry == nullptr ) {
                return RES_ERR_INVALID;
        }
        rec->ientry = info.ifound;
        struct res_entry* entries = alg_array(list, &stg->entry);
        entry = &entries[info.ifound];
        assert ( entry->state != RESOURCE_INVALID );
        if ( entry->state == RESOURCE_LOADING ) {
                return RES_ERR_READ_BLOCKED;
        }

        rec->byte_ori = entry->buff;
        rec->byte_curr = entry->buff;
        rec->byte_end = alloc_var_get_last ( entry->buff ) + 1;
        return RES_ERR_NORMAL;
}

enum RES_ERROR stager_reset_layer ( struct entry_record *rec )
{
        /* Retrieve the entry */
        struct stager *stg = rec->stg;
        struct res_entry* entries = alg_array(list, &stg->entry);
        struct res_entry* entry = &entries[rec->ientry];
        assert ( entry->state != RESOURCE_INVALID );
        if ( entry->state == RESOURCE_LOADING ) {
                return RES_ERR_READ_BLOCKED;
        }
        stg->byte_used -= alloc_var_get_len ( entry->buff );
        alloc_var_flush ( entry->buff );
        return RES_ERR_NORMAL;
}

enum RES_ERROR stager_delete_node ( struct entry_record *rec )
{
        return -1;
}

void stager_reset_data ( struct entry_record *rec )
{
        rec->byte_curr = rec->byte_ori;
}

/* Get next data element from resource piler */
void *stager_pop_data ( int size, struct entry_record *rec )
{
        untyped *curr = rec->byte_curr;
        DebugSession (
                struct stager *stg = rec->stg;
                struct res_entry* entries = alg_array(list, &stg->entry);
                struct res_entry* entry = &entries[rec->ientry];
                assert ( entry->state != RESOURCE_LOADING );
                assert ( curr >= rec->byte_ori && curr < rec->byte_end );
        )
        rec->byte_curr += size;
        return curr;
}
