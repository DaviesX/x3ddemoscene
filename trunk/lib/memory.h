#ifndef X3DMEMORYFILLER_H_INCLUDED
#define X3DMEMORYFILLER_H_INCLUDED

#include <x3d/common.h>

/*
 * Definitions
 */

#ifdef X3D_DEBUG_MODE
#  define MEMORY_INITIAL_COUNT			0
#else
#  define MEMORY_INITIAL_COUNT			64
#endif

#define MEMORY_LOCK_MASK			0X7FFFFFFF
#define MEM_NUM_UNKNOWN				0


// Utility macro function
#define alloc_fix( _elm_size, num_elm )		(call_alloc_fix ( _elm_size, num_elm))
#define free_fix( _ptr ) \
{ \
	call_free_fix ( _ptr ); \
	_ptr = nullptr; \
}
#define alloc_var( _elm_size, num_elm )		(call_alloc_var ( _elm_size, num_elm))
#define free_var( _ptr ) \
{ \
	call_free_var ( _ptr ); \
	_ptr = nullptr; \
}
#define add_var( _ptr, _n )			(call_add_var ( _ptr, _n ))
#define push_var( _data, _ptr )			(call_push_var ( _data, _ptr ))
#define pop_var( _ptr, _data )			(call_pop_var ( _ptr, _data ))
#define expand_var( _ptr, _n )			(call_expand_var( _ptr, _n ))
#define expand2_var( _ptr, _n )			(call_expand2_var( _ptr, _n ))
#define shrink_var( _ptr )			(call_shrink_var ( _ptr ))
#define dec_var( _ptr, _n )			(call_dec_var ( _ptr, _n ))
#define flush_var( _ptr )			(call_flush_var ( _ptr ))
#define get_var_len( _ptr )			(call_get_var_len ( _ptr ))
#define get_var_last( _ptr )			(call_get_var_last ( _ptr ))

#define alloc_obj( _data ) \
{ \
	alloc_fix(sizeof *(_data), 1); \
	memset ( _data, 0, sizeof *(_data) ); \
}
#define zero_obj( _data )			(memset ( _data, 0, sizeof(*(_data)) ))
#define zero_array( _data, _count )		(memset ( _data, 0, sizeof(*(_data))*(_count) ))


/* memory operations */
#define set_byte( _data, _dest )	(*(uint8_t *) (_dest) = *(uint8_t *) (_data))
#define set_word( _data, _dest )	(*(uint16_t *) (_dest) = *(uint16_t *) (_data))
#define set_long( _data, _dest )	(*(uint32_t *) (_dest) = *(uint32_t *) (_data))
#define set_double( _data, _dest )	(*(uint64_t *) (_dest) = *(uint64_t *) (_data))
#define set_quad( _data, _dest )

#define memsetb( _dest, _data, _count )		(memset ( _dest, _data, _count ))

/* Fill memory in word, notice that count is not equal to size in byte */
#define memsetw( _dest, _data, _count )		\
	asm (\
		"movl %0, %%edi\n"\
		"movl %1, %%ecx\n"\
		"movw %2, %%ax\n"\
		"rep stosw\n"\
		: /* no output */\
		:"r"(_dest), "r"(_count), "r"(_data)\
	);

/* Fill memory in long, notice that count is not equal to size in byte */
#define memsetl( _dest, _data, _count )	\
	asm (\
		"movl %0, %%edi\n"\
		"movl %1, %%ecx\n"\
		"movw %2, %%eax\n"\
		"rep stosd\n"\
		: /* no output */\
		:"r"(dest), "r"(count), "r"(data)\
	);

#define memsetd( _dest, _data, _count )
#define memsetq( _dest, _data, _count )

struct mem_block {
};

/*
 * Functions declarations
 */

pseudo_def ( void memsetw ( void *dest, uint16_t data, int count ); )
pseudo_def ( void memsetl ( void *dest, uint32_t data, int count ); )
pseudo_def ( void memsetll ( void *dest, uint64_t data, int count ); )
pseudo_def ( void memsetql ( void *dest, uint64_t data[2], int count ); )

void *call_alloc_fix ( uint32_t elm_size, uint32_t num_elm );
void call_free_fix ( void *ptr );
void *call_alloc_var ( uint32_t elm_size, uint32_t num_elm );
void call_free_var ( void *ptr );
void *call_add_var ( void *ptr, int n );
void *call_push_var ( void *data, void *ptr );
void *call_pop_var ( void *ptr, void *data );
void *call_expand_var ( void *ptr, uint32_t n );
void *call_expand2_var ( void *ptr, uint32_t n );
void *call_shrink_var ( void *ptr );
void *call_dec_var ( void *ptr, int n );
void *call_flush_var ( void *ptr );
uint32_t call_get_var_len ( void *ptr );
void *call_get_var_last ( void *ptr );
uint32_t query_mem_usage ( void );
uint32_t query_mem_inst ( void );


#endif // X3DMEMORYFILLER_H_INCLUDED
