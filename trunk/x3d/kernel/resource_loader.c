/* resource_loader.c: general thread-safe resource handling(staging, loading and reload) */
#include <algorithm.h>
#include <staging.h>
#include <x3d/resource_loader.h>


#define MAX_TYPE	5

/* each type of resource owns ONE independent resource line */
struct res_line {
        enum RES_TYPE type;
        struct stager certi;		/* certificate to check resources that are under control */
        struct stager file_handle;	/* file and handle as the key to obtain _resource_ */
        struct stager name;		/* name as the key to obtain _handle_ */
        void *res_mem;			/* pooled memory for the resource */
};

struct res_loader {
        res_handle_t h_loader;
        int n_res;
        res_handle_t *h_res;
};

/* each certificated resource are started with this header */
struct res_header {
        char *name;
        uint32_t hfile;
        res_handle_t handle;
        enum RES_STATE res_state;
        uint32_t ref;
        void *res_ptr;
};

struct handle_list {
        int prev;		/* pointer to the prev and next handle in the array */
        int next;
};

struct res_ops {
        void *(*load) ( const char *filename, struct res_loader *loader );
        void *(*reload) ( const char *filename, void *res_ptr );	/* No reload() is required
									   to implemen for the time being */
        void *(*free) ( void *res_ptr );	/* returning nullptr indicates a successful release */
};

static struct res_line g_res_line[] = {
        [RES_TYPE_GEOMETRY].type = RES_TYPE_GEOMETRY,
        [RES_TYPE_KINEMATIC_SHAPE].type = RES_TYPE_KINEMATIC_SHAPE,
        [RES_TYPE_MATERIAL].type = RES_TYPE_MATERIAL,
        [RES_TYPE_SURFACE].type = RES_TYPE_SURFACE,
        [RES_TYPE_SHADER].type = RES_TYPE_SHADER
};

static struct res_ops g_res_ops[] = {
};

static struct handle_list *g_handle_list = nullptr;
static int g_curr_handle = 0;		/* pointer to the current handle and the recycled handle */
static int g_rec_handle = 0;

static struct stager g_loader;

static void optimize_res_line ( struct res_line *res );
static uint32_t obtain_unique_handle ( void );
static void return_unique_handle ( uint32_t handle );
static struct res_loader *init_loader ( void );
static void free_loader ( struct res_loader *loader );


static uint32_t obtain_unique_handle ( void )
{
        return 0;
}

static void return_unique_handle ( uint32_t handle )
{
}

static void optimize_res_line ( struct res_line *res )
{
}

static struct res_loader *create_loader ( void )
{
        struct res_loader *loader = alloc_fix ( sizeof ( *loader ), 1 );
        loader->h_loader = obtain_unique_handle ();
        loader->h_res = alloc_var ( sizeof ( res_handle_t ), 0 );
        loader->n_res = 0;
        return loader;
}

static void free_loader ( struct res_loader *loader )
{
}

void init_res_loader ( void )
{
        g_handle_list = alloc_var ( sizeof ( *g_handle_list ), 1 );
        g_handle_list[0].prev = 0;
        g_handle_list[0].next = 0;
        int i;
        for ( i = 0; i < MAX_TYPE; i ++ ) {
                init_stager ( &g_res_line[i].certi );
                init_stager ( &g_res_line[i].file_handle );
                init_stager ( &g_res_line[i].name );
        }
        init_stager ( &g_loader );
}

void free_res_loader ( void )
{
        int i;
        for ( i = 0; i < MAX_TYPE; i ++ ) {
                free_stager ( &g_res_line[i].certi );
                free_stager ( &g_res_line[i].file_handle );
                free_stager ( &g_res_line[i].name );
        }
        free_var ( g_handle_list );
        free_loader ( &g_loader );
}

enum RES_STATE load_resource ( char *filename, bool is_sync, struct res_loader **loader )
{
        struct res_loader *t_loader = create_loader ();
        *loader = t_loader;

        /* check if the resource had been certified */
        /* open an new entry and fill in header */

        enum RES_TYPE type;
        switch ( type ) {
        case RES_TYPE_GEOMETRY: {
                if ( is_sync ) {
                        if ( g_res_ops[RES_TYPE_GEOMETRY].load ( filename, t_loader ) ) {
                                return RES_LOADED;
                        } else {
                                return RES_LOAD_FAIL;
                        }
                } else {
                        return RES_LOADING;
                }
                break;
        }
        default: {
                return RES_LOAD_FAIL;
        }
        }
}

void reload_resource ( struct res_header *header )
{
}

void free_resource ( struct res_header *header )
{
}

enum RES_STATE checkout_resource ( res_ptr_t *res )
{
        return 0;
}

enum RES_STATE checkout_res_from_loader ( struct res_loader *loader, int i,
                struct res_header **header )
{
        return 0;
}

void free_loader_resource ( res_loader_t loader )
{
        free_loader ( loader );
}

/* Call by inserting a res_ptr on heap memory. Save the res_header for  */
void add_resource ( void *res_ptr, enum RES_TYPE type, struct res_header *header )
{
}

void add_to_res_loader ( void *res_ptr, enum RES_TYPE type, struct res_loader *loader )
{
}
