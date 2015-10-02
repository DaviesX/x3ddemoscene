#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

#include <x3d/common.h>
#include <misc.h>

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
#define free_fix( _ptr ) \
{ \
	__free_fix(_ptr); \
	_ptr = nullptr; \
}

#define free_var(_ptr) \
{ \
	__free_var(_ptr); \
	_ptr = nullptr; \
}

#define alloc_obj(_data) \
	alloc_fix(sizeof *(_data), 1); \
	memset ( _data, 0, sizeof *(_data) ); \

#define zero_obj( _data )			(memset(_data, 0, sizeof(*(_data))))
#define zero_array( _data, _count )		(memset(_data, 0, sizeof(*(_data))*(_count)))


/* memory operations */
#define set_byte( _data, _dest )	(*(uint8_t *) (_dest) = *(uint8_t *) (_data))
#define set_word( _data, _dest )	(*(uint16_t *) (_dest) = *(uint16_t *) (_data))
#define set_long( _data, _dest )	(*(uint32_t *) (_dest) = *(uint32_t *) (_data))
#define set_double( _data, _dest )	(*(uint64_t *) (_dest) = *(uint64_t *) (_data))
#define set_quad( _data, _dest )

#define memsetb(_dest, _data, _count)		(memset(_dest, _data, _count))

/* Fill memory in word, notice that count is not equal to size in byte */
#define memsetw(_dest, _data, _count)		\
	asm (\
		"movl %0, %%edi\n"\
		"movl %1, %%ecx\n"\
		"movw %2, %%ax\n"\
		"rep stosw\n"\
		: /* no output */\
		:"r"(_dest), "r"(_count), "r"(_data)\
	);

/* Fill memory in long, notice that count is not equal to size in byte */
#define memsetl(_dest, _data, _count)	\
	asm (\
		"movl %0, %%edi\n"\
		"movl %1, %%ecx\n"\
		"movw %2, %%eax\n"\
		"rep stosd\n"\
		: /* no output */\
		:"r"(dest), "r"(count), "r"(data)\
	);

#define memsetd(_dest, _data, _count)
#define memsetq(_dest, _data, _count)

struct arena_allocator {
};

/*
 * Functions declarations
 */

PseudoDef(void memsetw(void *dest, uint16_t data, int count);)
PseudoDef(void memsetl(void *dest, uint32_t data, int count);)
PseudoDef(void memsetll(void *dest, uint64_t data, int count);)
PseudoDef(void memsetql(void *dest, uint64_t data[2], int count);)

void* 		alloc_fix(uint32_t elm_size, uint32_t num_elm);
void 		__free_fix(void* ptr);
void*		alloc_var(uint32_t elm_size, uint32_t num_elm);
void 		__free_var(void* ptr);
void*		alloc_add_var(void* ptr, int n);
void* 		alloc_push_var(void* data, void* ptr);
void*		alloc_pop_var(void* ptr, void* data);
void*		alloc_expand_var(void* ptr, uint32_t n);
void*		alloc_expand2_var(void* ptr, uint32_t n);
void*		alloc_shrink_var(void* ptr);
void*		alloc_dec_var(void* ptr, int n);
void*		alloc_flush_var(void* ptr);
uint32_t 	alloc_get_var_len(void* ptr);
void*		alloc_get_var_last(void* ptr);
uint32_t 	alloc_query_usage();
uint32_t 	alloc_query_instance_count();


#endif // ALLOCATOR_H_INCLUDED
