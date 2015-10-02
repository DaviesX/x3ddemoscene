/* allocator.c: Memory managment */
#include <x3d/common.h>
#include <misc.h>
#include <system/log.h>
#include <system/allocator.h>


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

static inline void inc_inst()
{
	static int ibudget = 1;
	DebugIf ( g_mem_rec.inst >= INSTANCE_BUGDET*ibudget,
			log_mild_err_dbg ( "Running over instance budget: %d",
					   INSTANCE_BUGDET*ibudget );
			ibudget ++;
		)
	g_mem_rec.inst ++;
}

static inline void dec_inst()
{
	g_mem_rec.inst --;
}

static struct var_head* change_size_var(struct var_head* vh, int new_size, int factor);


void* alloc_fix(uint32_t elm_size, uint32_t num_elm)
{
        if (elm_size == 0 || num_elm == 0)
                return nullptr;
        uint32_t size = elm_size*num_elm;

        struct fix_head fh = {
                .used = size
        };
        const uint32_t offset = sizeof(fh);
        untyped *mem = malloc(size + offset);
        assert(mem);
        inc_usage(size + offset);
        inc_inst();

        memcpy(mem, &fh, offset);
        return &mem[offset];
}

void __free_fix(void* ptr)
{
        if (ptr) {
                struct fix_head* fh = &((struct fix_head*) ptr)[-1];
                dec_usage(fh->used + sizeof (*fh));
                dec_inst();
                free(fh);
        }
}

/* Allocate variable memory */
void *alloc_var ( uint32_t elm_size, uint32_t num_elm )
{
        assert ( elm_size != 0 );
        num_elm = (num_elm == 0) ? INIT_COUNT : num_elm;

        struct var_head vh;
        uint32_t size = elm_size*num_elm;
        const uint32_t offset = sizeof ( vh );
        untyped *mem = malloc ( size + offset );
        assert ( mem );
        inc_usage ( size + offset );
        inc_inst ();

        vh.elm_size = elm_size;
        vh.used = 0;
        vh.total = size;

        memcpy ( mem, &vh, offset );
        return &mem[offset];
}

void __free_var(void* ptr)
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
        inc_inst();

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
void *alloc_add_var ( void *ptr, int n )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        if ( vh->used + n*vh->elm_size > vh->total ) {
                vh = change_size_var ( vh, vh->used + n*vh->elm_size, 2 );
        }
        vh->used += n*vh->elm_size;
        return &vh[1];
}

void *alloc_push_var ( void *data, void *ptr )
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

void* alloc_pop_var(void* ptr, void* data)
{
        struct var_head* vh     = &((struct var_head*) ptr)[-1];
        vh->used                -= vh->elm_size;
        untyped* buff           = (untyped*) &vh[1];
        memcpy(data, &buff[vh->used], vh->elm_size);
        return buff;
}

void *alloc_expand_var ( void *ptr, uint32_t n )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        if ( n*vh->elm_size > vh->total ) {
                vh = change_size_var ( vh, n*vh->elm_size, 1 );
        }
        vh->used = n*vh->elm_size;
        return &vh[1];
}

void *alloc_expand2_var ( void *ptr, uint32_t n )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        if ( n*vh->elm_size > vh->total ) {
                vh = change_size_var ( vh, n*vh->elm_size, 2 );
        }
        vh->used = n*vh->elm_size;
        return &vh[1];
}

void *alloc_shrink_var ( void *ptr )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        if ( vh->total > vh->used ) {
                vh = change_size_var ( vh, vh->used, 1 );
        }
        return &vh[1];
}

void *alloc_dec_var ( void *ptr, int n )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        vh->used -= n*vh->elm_size;
        return &vh[1];
}

void *alloc_flush_var ( void *ptr )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        vh->used = 0;
        return &vh[1];
}

uint32_t alloc_get_var_len ( void *ptr )
{
        struct var_head *vh = &((struct var_head *) ptr)[-1];
        return vh->used/vh->elm_size;
}

void *alloc_get_var_last ( void *ptr )
{
        struct var_head* vh     = &((struct var_head*) ptr)[-1];
        ptr                     = (untyped*) ptr + vh->used - vh->elm_size;
        address_t addr          = (vh->used != 0)*(*(uint32_t *) &ptr);
        return (void*) addr;
}

uint32_t alloc_query_usage ( void )
{
        return g_mem_rec.used;
}

uint32_t alloc_query_instance_count ( void )
{
        return g_mem_rec.inst;
}


// Test Cases:
#include <container/paramset.h>
#include <x3d/debug.h>
// variable_memory_test0
__dlexport void __callback                  variable_memory_test0_init(struct alg_var_set* envir) {}
__dlexport void __callback                  variable_memory_test0_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   variable_memory_test0_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  variable_memory_test0(struct alg_var_set* envir)
{
        char **str_arr = alloc_var(sizeof(char*), 0);
        char *s0 = "Hello, first string\n";
        str_arr = alloc_push_var(&s0, str_arr);
        char *s1 = "Do you see me ? Second string\n";
        str_arr = alloc_push_var(&s1, str_arr);
        log_normal("%s", &str_arr[0][0]);
        log_normal("%s", &str_arr[1][0]);
        free_var(str_arr);
}

// variable_memory_test1
__dlexport void __callback                  variable_memory_test1_init(struct alg_var_set* envir) {}
__dlexport void __callback                  variable_memory_test1_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   variable_memory_test1_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        return variable_memory_test0_pos(envir, n_pos, num_run, is_skipped);
}
__dlexport void __callback              variable_memory_test1(struct alg_var_set* envir)
{
        log_normal("memory query 0: %db", alloc_query_usage ());
        const int count = 10000;
        /* add/decrease */
        int *array = alloc_var(sizeof(int), 0);
        log_normal("memory query 1: %db", alloc_query_usage ());
        int i;
        for(i = 0; i < count; i ++) {
                array = alloc_add_var(array, 1);
                array[i] = i;

                log_set_behavior(LOG_OUTPUT_TO_FILE);
                log_normal("%d", array[i]);
        }
        log_normal("number element(added): %d", alloc_get_var_len(array));
        for(i = 0; i < count/2; i ++) {
                array = alloc_dec_var(array, 2);
        }
        log_normal("number element(deleted): %d", alloc_get_var_len(array));
        log_normal("memory query 2: %db", alloc_query_usage ());
        /* shrink-expand */
        array = alloc_shrink_var(array);
        log_normal("number element(shrinked): %d", alloc_get_var_len(array));
        log_normal("memory query 3: %db", alloc_query_usage ());
        for(i = 0; i < count; i ++) {
                array = alloc_expand_var(array, i + 1);
                array[i] = i;
        }
        log_normal("number element(expanded): %d", alloc_get_var_len(array));
        alloc_flush_var(array);
        log_normal("number element(flushed): %d", alloc_get_var_len(array));
        free_var(array);
        log_normal("memory query 4: %db", alloc_query_usage ());
}
