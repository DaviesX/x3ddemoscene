/* X3dMemory.c: Memory managment */
#include <logout.h>
#include <memory.h>


#define INIT_COUNT			1
#define INSTANCE_BUGDET			1024

/* fixed length memory */
struct fix_head {
	uint32_t used;
};

/* variable length memory */
struct var_head {
	uint32_t used;
	uint32_t elm_size;
	uint32_t total;
};

/* pooled memory */
struct pool_head {
};

struct mem_rec {
	uint32_t used;
	uint32_t inst;
};

struct mem_rec g_mem_rec = {0};

#define inc_usage( _size )		(g_mem_rec.used += (_size))
#define dec_usage( _size )		(g_mem_rec.used -= (_size))
#define inc_inst() \
{ \
	static int ibudget = 1; \
	DEBUG_IF ( g_mem_rec.inst >= INSTANCE_BUGDET*ibudget, \
			log_mild_err_dbg ( "Running over instance budget: %d", \
					   INSTANCE_BUGDET*ibudget ); \
			ibudget ++; \
		) \
	g_mem_rec.inst ++; \
}
#define dec_inst() \
{ \
	g_mem_rec.inst --; \
}

struct var_head *change_size_var ( struct var_head *vh, int new_size, int factor );


void *call_alloc_fix ( uint32_t elm_size, uint32_t num_elm )
{
	assert ( elm_size != 0 && num_elm != 0 );
	uint32_t size = elm_size*num_elm;

	struct fix_head fh = {
		.used = size
	};
	const uint32_t offset = sizeof ( fh );
	untyped *mem = malloc ( size + offset );
	assert ( mem );
	inc_usage ( size + offset );
	inc_inst ();

	memcpy ( mem, &fh, offset );
	return &mem[offset];
}

void call_free_fix ( void *ptr )
{
	if ( ptr ) {
		struct fix_head *fh = &((struct fix_head *) ptr)[-1];
		dec_usage ( fh->used + sizeof ( *fh ) );
		dec_inst ();
		free ( fh );
	}
}

/* Allocate variable memory */
void *call_alloc_var ( uint32_t elm_size, uint32_t num_elm )
{
	assert ( elm_size != 0 );
	num_elm = (num_elm == 0) ? INIT_COUNT : num_elm;

	struct var_head vh;
	uint32_t size = elm_size*num_elm;
	const uint32_t offset = sizeof ( vh );
	untyped *mem = malloc ( size + offset );
	assert ( mem );
	inc_usage ( size + offset );
	inc_inst ()

	vh.elm_size = elm_size;
	vh.used = 0;
	vh.total = size;

	memcpy ( mem, &vh, offset );
	return &mem[offset];
}

void call_free_var ( void *ptr )
{
	if ( ptr ) {
		struct var_head *vh = &((struct var_head *) ptr)[-1];
		dec_usage ( vh->total + sizeof ( *vh ) );
		dec_inst ();
		free ( vh );
	}
}

struct var_head *change_size_var ( struct var_head *vh, int new_size, int factor )
{
	void *mem = malloc ( factor*new_size + sizeof ( *vh ) );
	assert ( mem );
	inc_usage ( factor*new_size + sizeof ( *vh ) );
	inc_inst ()

	struct var_head *vh2 = mem;
	vh2->elm_size = vh->elm_size;
	vh2->total = factor*new_size;
	vh2->used = vh->used;
	memcpy ( &vh2[1], &vh[1], vh->used );

	dec_usage ( vh->total + sizeof ( *vh ) );
	dec_inst ();
	free ( vh );
	return vh2;
}

/* Increase the size of a variable memory,
 * and return the new memory address after the increment.
 * You _MUST_ retrieve the returned value*/
void *call_add_var ( void *ptr, int n )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	if ( vh->used + n*vh->elm_size > vh->total ) {
		vh = change_size_var ( vh, vh->used + n*vh->elm_size, 2 );
	}
	vh->used += n*vh->elm_size;
	return &vh[1];
}

void *call_push_var ( void *data, void *ptr )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	if ( vh->used + vh->elm_size > vh->total ) {
		vh = change_size_var ( vh, vh->used + vh->elm_size, 2 );
	}
	untyped *buff = (untyped *) &vh[1];
	memcpy ( &buff[vh->used], data, vh->elm_size );
	vh->used += vh->elm_size;
	return buff;
}

void *call_pop_var ( void *ptr, void *data )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	vh->used -= vh->elm_size;
	untyped *buff = (untyped *) &vh[1];
	memcpy ( data, &buff[vh->used], vh->elm_size );
	return buff;
}

void *call_expand_var ( void *ptr, uint32_t n )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	if ( n*vh->elm_size > vh->total ) {
		vh = change_size_var ( vh, n*vh->elm_size, 1 );
		vh->used = vh->total;
	}
	return &vh[1];
}

void *call_expand2_var ( void *ptr, uint32_t n )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	if ( n*vh->elm_size > vh->total ) {
		vh = change_size_var ( vh, n*vh->elm_size, 2 );
		vh->used = vh->total;
	}
	return &vh[1];
}

void *call_shrink_var ( void *ptr )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	if ( vh->total > vh->used ) {
		vh = change_size_var ( vh, vh->used, 1 );
	}
	return &vh[1];
}

void *call_dec_var ( void *ptr, int n )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	vh->used -= n*vh->elm_size;
	return &vh[1];
}

void *call_flush_var ( void *ptr )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	vh->used = 0;
	return &vh[1];
}

uint32_t call_get_var_len ( void *ptr )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	return vh->used/vh->elm_size;
}

void *call_get_var_last ( void *ptr )
{
	struct var_head *vh = &((struct var_head *) ptr)[-1];
	ptr = (untyped *) ptr + vh->used - vh->elm_size;
	uint32_t addr = (vh->used != 0)*(*(uint32_t *) &ptr);
	return (void *) addr;
}

uint32_t query_mem_usage ( void )
{
	return g_mem_rec.used;
}

uint32_t query_mem_inst ( void )
{
	return g_mem_rec.inst;
}
