#ifndef ALGORITHM_H_INCLUDED
#define ALGORITHM_H_INCLUDED


/*
 * Definitions
 */

#include <x3d/common.h>
#include <math/math.h>

#define ALG_HASH_INVALID	HASH_NULL_KEY

/* Utility macro functions */
#define init_cdecl_stack( _stack ) \
{ \
	(_stack)->stack_ptr = (_stack)->tmp_stack; \
	/* (_stack)->n_ref = 0; */ \
}
#define push_cdecl_stack( _stack, _data ) \
{ \
	*(typeof (_data) *)(_stack)->stack_ptr = (_data); \
	(_stack)->stack_ptr += sizeof ( typeof (_data) ); \
}
#define start_cdecl_stack			init_cdecl_stack
#define pop_cdecl_stack( _stack, _value ) \
{ \
	(_value) = *((typeof (_value) *) (_stack)->stack_ptr); \
	(_stack)->stack_ptr += sizeof ( typeof (_value) ); \
}
#define end_cdecl_stack				init_cdecl_stack

#define alg_list_n( _list )			((_list)->num_elm)
#define alg_list_first( _list )			((void *) (_list)->list)
#define alg_list_next( _list_elm )		((_list_elm) ++)
#define alg_list_i( _list, _k, _ptr )           (*(_ptr) = &((typeof(*(_ptr))) (_list)->list)[_k])
#define get_alg_cont_header( _container )	((untyped *) (_container) + sizeof ( struct container_operations ))
#define set_alg_cont_header( _ptr, _container )	\
	(memcpy ( (untyped *) (_container) + sizeof ( struct container_operations ), (_ptr), sizeof ( _ptr ) ))

#define begin_tree_traversal( _id ) \
{ \
	TRAVERSE_BEGIN_##_id: /* begins */; \
}
#define end_tree_traversal( _id ) \
{ \
	goto TRAVERSE_BEGIN_##_id;\
	TRAVERSE_END_##_id: /* ends */; \
}
#define enter_tree_branch( _enter_code ) \
{\
	_enter_code\
}
#define backtrack_tree_branch( _stack, _id, _backtrack_code ) \
{ \
	if ( (_stack)->stack_ptr == (untyped *) (_stack) ) { \
		goto TRAVERSE_END_##_id; \
	} else { \
		_backtrack_code \
		goto TRAVERSE_BEGIN_##_id; \
	} \
}

#define alg_ptr_list_i( _ptr_l, i )	((_ptr_l)[i].ptr)


/*
 * Structures
 */

// Index List, which uses subscript pointing to the data list
typedef struct ALG_INDEX_LIST_TYP {
        int index;
} ALG_INDEX_LIST, *ALG_INDEX_LIST_PTR;

// Index list that has a reference which points to its original place
typedef struct ALG_BIINDEX_LIST_TYP {
        int index;
        int origin;
} ALG_BIINDEX_LIST, *ALG_BIINDEX_LIST_PTR;

/* Sometimes we need to put the pointers of
 * an array of data into a separate list for non-linear
 * access or arrangment temporarily or perminently. When the
 * process is done, the alg_pointer_list can restore the lineary order
 * with a new copy of array, while alg_double_pointer_list does not
 * both restoring process are O(n) */
struct alg_pointer_list {
        void *ptr;
};

struct alg_double_pointer_list {
};

/* Defines a small stack for cdecl-like pushes */
struct alg_cdecl_stack {
        untyped tmp_stack[256];
        untyped *stack_ptr;
};

/* A small parameters lookup table */
struct alg_named_params {
        int pop_state;
        char **str;
        int *ref;
        int n_ref;
        int size;
        untyped *buff;
        int curr_pos;
};

typedef uint64_t uuid_t;

#define init_named_params( _p ) \
{ \
	memset ( _p, 0, sizeof ( *(_p) ) ); \
	(_p)->str = alloc_var ( sizeof(char*), 0 ); \
	(_p)->ref = alloc_var ( sizeof(int), 0 ); \
	(_p)->buff = alloc_var ( sizeof(untyped), 0 ); \
}

#define push_named_params( _data, _name, _p ) \
{ \
	typeof (_data) _d = _data; \
\
	/* Check if I've got new stuffs */ \
	int _i = -1; \
	int eq = 1; \
	while ( ++ _i < (_p)->n_ref && (eq = strcmp ( _name, (_p)->str[_i] )) ); \
	if ( eq != 0 ) { \
		/* It does have new stuffs */ \
		(_p)->str = add_var ( (_p)->str, 1 ); \
		(_p)->str[(_p)->n_ref] = alloc_fix ( 1, strlen ( _name ) + 1 ); \
		strcpy ( (_p)->str[(_p)->n_ref], _name ); \
	 \
		(_p)->ref = add_var ( (_p)->ref, 1 ); \
		(_p)->ref[(_p)->n_ref] = (_p)->size; \
		(_p)->n_ref ++; \
	 \
		(_p)->buff = add_var ( (_p)->buff, sizeof (_d) ); \
		memcpy ( &(_p)->buff[(_p)->size], &(_d), sizeof (_d) ); \
		(_p)->size += sizeof (_d); \
	} else { \
		/* No, I'd better modify the old one */ \
		memcpy ( &(_p)->buff[(_p)->ref[_i]], &(_d), sizeof (_d) ); \
	} \
}

#define push_params( _data, _p ) \
{ \
	typeof (_data) _d = _data; \
	(_p)->buff = add_var ( (_p)->buff, sizeof (_d) ); \
	memcpy ( &(_p)->buff[(_p)->size], &(_d), sizeof (_d) ); \
	(_p)->size += sizeof (_d); \
}

#define pop_named_params( _p, _name, _data ) \
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

#define pop_params( _p, _data ) \
{ \
	(_data) = *(typeof (_data) *) &(_p)->buff[(_p)->curr_pos]; \
	(_p)->curr_pos += sizeof (_data); \
}

#define reset_named_params( _p ) \
{ \
	int _i; \
	for ( _i = 0; _i < (_p)->n_ref; _i ++ ) { \
		free_fix ( (_p)->str[_i] ); \
		(_p)->str[_i] = nullptr; \
	} \
	flush_var ( (_p)->str ); \
	flush_var ( (_p)->ref ); \
	flush_var ( (_p)->buff ); \
	(_p)->n_ref = 0; \
	(_p)->size = 0; \
	(_p)->curr_pos = 0; \
}

#define check_named_params( _p )	((_p)->pop_state != 0)

#define free_named_params( _p ) \
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

/* stimulate an asm-free stack which
 * can be declared with custom settings */
#define declare_stack( _name, _size )	\
	struct {\
		untyped stack[_size];\
		untyped *stack_ptr;\
	} _name;

#define init_stack( _stack )		((_stack)->stack_ptr = (untyped *) (_stack))
#define push_stack( _stack, _a ) \
{ \
	*(typeof (_a) *) (_stack)->stack_ptr = _a; \
	(_stack)->stack_ptr += sizeof ( _a ); \
	assert ( (_stack)->stack_ptr <= (_stack)->stack + sizeof ( (_stack)->stack ) ); \
}

#define pop_stack( _stack, _a ) \
{ \
	(_stack)->stack_ptr -= sizeof ( _a ); \
	assert ( (_stack)->stack_ptr >= (_stack)->stack ); \
	_a = *(typeof (_a) *) (_stack)->stack_ptr; \
}

#define stack_is_empty( _stack )	((_stack)->stack_ptr == (_stack)->stack)

#define get_stack_last( _stack, _a ) \
{ \
	untyped *t = (_stack)->stack_ptr - sizeof ( _a ); \
	_a = *(typeof (_a) *) t; \
}

#define check_stack( _stack )		((_stack)->stack != (_stack)->stack_ptr)

#pragma pack(4)
struct container_operations {
        void (*init_callback) ( void *elm );
        void (*destroy_callback) ( void *elm );
};

/* Provides a list container and
 * some common O(n) operations below */
struct alg_list {
        struct container_operations ops;
        untyped *list;
        int elm_size;
        int num_elm;
};

/* Provides a linked list hash table container
 * Some common O(1) operations are defined below */
struct alg_hash_link_list {
        int *hash_table;	/* Table header and the follow data linked list */
        int header_len;
        int link_list_len;
        int shift_bits;
        int size_bits;
};

struct alg_hash_table {
        int *hash_table;
        int len;
};

/* linked list */
struct alg_llist {
        int *head;
        int *prev, *next;
        int ilast, icurr;
        int elm_size;
        int num_elm;
        void *content;
};

/* Provides a balance tree container and
 * some common O(log n) operations below */
struct alg_btree {
};

struct alg_llist_btree {
};

#pragma pack()


/*
 * Functions' declaration
 */

// Sorting an array list with bubble sort algorithm
pseudo_def ( SortArrayBubble( _list, _length, _TYPE, _info, _CompareFunction ) )

// Sorting an array list with insertion sort algorithm
pseudo_def ( SortArrayInsertion( _list, _length, _TYPE, _info, _CompareFunction ) )

// Sorting a list with quick sort algorithm
pseudo_def ( SortArrayQuick( _list, _length, _TYPE, _info, _CompareFunction ) )

// Partition an array list in-place according to the given mid-value
pseudo_def ( PartitionArrayMidValue( _list, _length, _TYPE, _midValue, _CompareFunction, _midIndex ) )
pseudo_def ( void alg_divide_array ( void *array, int length, void *info,
                                     int (*compare) ( void *current, void *info ),
                                     int *imiddle ); )

// Partition an array list in-place without a given mid-value
pseudo_def ( PartitionArray( _list, _length, _TYPE, _info, _CompareFunction, _midIndex ) )

// Find the nth element in the array list
pseudo_def ( FindNthElementArray( _list, _length, _nthPos, _TYPE, _info, _CompareFunction ) )
pseudo_def ( alg_split_array ( void *array, int length, int i, void *info,
                               int (*compare) ( void *elm0, void *elm1, void *info ) ) )

// Create a indirect index list for different list-series algorithm
pseudo_def ( CreateIndexList( _indexList, _length ) )

// Copy the array to the new place according to the ordered index list
pseudo_def ( ProcessIndexListToArray( _list, _indexList, _length, _TYPE, _newList ) )

// Create a indirect bi-index list for different list-series algorithm
pseudo_def ( CreateBiindexList( _indexList, _length ) )

// Rearrage the whole array list according to the ordered bi-index list
pseudo_def ( ProcessBiindexList( _list, _indexList, _length, _TYPE ) )

// Sorting an index list with bubble sort algorithm
pseudo_def ( SortIndexBubble( _list, _indexList, _length, _TYPE, _info, _CompareFunction, Copier ) )

// Sorting an index list with insertion sort algorithm
pseudo_def ( SortIndexInsertion( _list, _indexList, _length, _TYPE, _info, _CompareFunction ) )

// Sorting an index list with quick sort algorithm
pseudo_def ( SortIndexQuick( _list, _indexList, _length, _TYPE, _info, _CompareFunction ) )

// Partition an index list in-place according to the given mid-value
pseudo_def ( PartitionIndexMidValue( _list, _indexList, _length, _midValue, _CompareFunction, _midIndex ) )

// Partition an index list in-place without a given mid-value
pseudo_def ( PartitionIndex( _list, _indexList, _length, _info, _CompareFunction, _midIndex ) )

// Find the nth element in a index list
pseudo_def ( FindNthElementIndex( _list, _indexList, _length, _nthPos, _TYPE, _info, _CompareFunction ) )

// Initialize and create the hash table
pseudo_def ( InitHashTableStatic( _tablePowerCount, _srcArrayCount, _hashTable ) )

// Release allocated memory in the hash table
pseudo_def ( ReleaseHashTableStatic( _hashTable ) )

// Empty all the elements from hash table
pseudo_def ( FlushHashTableStatic( _hashTable ) )

// Look up if the data has been inserted into the table, if not, inserts it
pseudo_def ( FindAddElementHashStatic( _hashTable, _dataIndex, _hashKey, _foundPos, _info, _CompareFunction ) )

// Look up if the data has been inserted into the table, if not, inserts it, if so, it tries to keep records with it
pseudo_def ( FindAddElementHashStaticX( _hashTable, _dataIndex, _hashKey, _foundPos, _info, _CompareFunction ) )

int alg_match_pattern ( const char *cstr, const char *format, ... );

int strn_to_int ( const char *str, int *iValue, int n );
int strn_to_float ( const char *str, float *fValue, int n );

uint32_t alg_hash_str0 ( char *str );
uint32_t alg_hash_str1 ( char *str );
uint32_t alg_hash_str2 ( char *str );
uint32_t alg_hash_str3 ( char *str );
uint32_t alg_hash_str4 ( char *str );
uuid_t alg_hash_str_uuid ( char *str );

int alg_match_substring ( char *str, char *sub );
void alg_init_kmp_substring ( char *sub, char *kmp_array[] );
int alg_match_substring_kmp ( char *str, char *sub, char *kmp_array[] );

char *uint_to_str_tens ( unsigned int n, char *str, int len );
char *uint_to_str_hex ( unsigned int n, char *str, int len );

void set_container_operations ( void *container,
                                void (*init_callback) ( void *elm ),
                                void (*destroy_callback) ( void *elm ) );

void create_alg_list ( struct alg_list *list, int elm_size, int init_count );
void free_alg_list ( struct alg_list *list );
void alg_list_add ( void *elm, struct alg_list *list );
void alg_list_expand ( int count, struct alg_list *list );
void alg_list_flush ( struct alg_list * list );
pseudo_def ( void alg_list_find ( struct alg_list *list, void *info, void **elm_data,
                                  uint8_t (*compare_func) ( void *info, void *elm ) ); )
pseudo_def ( void alg_list_remove ( struct alg_list *list, void *info,
                                    uint8_t (*compare_func) ( void *info, void *elm ) ); )
pseudo_def ( int alg_list_n ( struct alg_list *list ); )
void alg_list_copy ( struct alg_list *list0, struct alg_list *list1 );
void alg_list_swap ( struct alg_list *list0, struct alg_list *list1 );

void create_ptr_list ( struct alg_list *list, struct alg_pointer_list **ptrs );
void free_ptr_list ( struct alg_pointer_list *ptrs );
pseudo_def ( void process_ptr_list ( struct alg_pointer_list *ptrs, struct alg_list *new_list ) )

void create_alg_llist ( struct alg_llist *llist, int elm_size );
void free_alg_llist ( struct alg_llist *llist );
pseudo_def ( void alg_llist_i ( struct alg_llist *llist, int i, void **elm ); )
pseudo_def ( void alg_llist_add ( void *elm, struct alg_llist *llist ); )
pseudo_def ( void alg_llist_push ( void *elm, struct alg_llist *llist ); )
pseudo_def ( void alg_llist_pop ( struct alg_llist *llist, void **elm ); )
pseudo_def ( void alg_llist_find ( struct alg_llist *llist, void *info, void **elm_data,
                                   bool (*compare_func) ( void *info, void *elm ) ); )
pseudo_def ( void alg_llist_remove ( struct alg_llist *llist, void *info, void **elm_data,
                                     bool (*compare_func) ( void *info, void *elm ) ); )
pseudo_def ( void alg_llist_remove_ptr ( void *elm, struct alg_llist *llist ); )
void *alg_llist_first ( struct alg_llist *llist, int *it );
void *alg_llist_next ( struct alg_llist *llist, int *it );
void *alg_llist_new ( struct alg_llist *llist );
void *alg_llist_recycle ( struct alg_llist *llist );
void alg_llist_flush ( struct alg_llist *list );

uuid_t alg_gen_uuid ( void );
void alg_use_uuid ( uuid_t id );


#include <algorithm.inc>


#endif // ALGORITHM_H_INCLUDED
