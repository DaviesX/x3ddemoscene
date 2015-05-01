/* resloader.c: general thread-safe resource handling(staging, loading, saving and reload) */
#include <system/allocator.h>
#include <system/log.h>
#include <system/thread.h>
#include <container/linkedlist.h>
#include <container/staging.h>
#include <x3d/resloader.h>
#include <x3d/modelentity.h>
#include <loader/modelloader.h>


#define MAX_RESOURCE	        10
#define SANITIZE_THRESHOLD      1000

struct res_task;
struct res_loader;

struct res_comp {
        struct res_loader *loader;

        struct {
                void *data;
                int elm_size;
                int count;
        } *vertex, *normal, *uv,
        *tangent;
        int nvertex, nnormal, nuv, ntangent;

        struct {
                void *data;
                int n_vert;
                int count;
        } *face;
        int nface;

        struct {
                void *data;
                char *name;
                int count;
        } *chars, *ints, *floats,
        *refptrs;
        int nchars, nints, nfloats, nrefptrs;

        struct {
                void *data;
                char *name;
                int elm_size;
                int x, y;
        } *image, *twodarray;
        int nimage, ntwodarray;
};

struct res_task {
        char*                   filename;
        enum RES_STATE          state;
        struct thread_task*        thrtask;
        enum RES_IDR            type;
        struct res_loader*      loader;
        struct res_comp*        stg0_info;
        res_ptr_t*              stg1_inst;
        int                     n_stg1;
        int                     n_ref;
};

struct res_loader {
        bool    is_valid;
        struct {
                d_alg_llist(res_task*)  task;
                struct stager           content;
                int                     n_task;
                float                   vacancy;
        } *task;
        int     pivot;
        int     total_task;
        struct arena_allocator  block;
        struct thread_trap         trap;
        struct work_group*      wgroup;
};

struct res_ops {
        bool (*load_stg0) ( char *filename, struct res_task *task );
        res_ptr_t (*load_stg1) ( struct res_comp *comp );
        res_ptr_t (*reload) ( char *filename, struct res_task *task );	/* No reload() is required to implemen for the time being */
        bool (*save_stg0) ( res_ptr_t ptr, struct res_task *task );
        bool (*save_stg1) ( char *filename, struct res_comp *comp );
        bool (*free_stg0) ( struct res_comp *comp );
        bool (*free_stg1) ( struct res_task *task );
};

static struct res_ops g_res_ops[] = {
        [RES_MODEL_ENTITY].load_stg0 = load_model_entity,
        [RES_MODEL_ENTITY].load_stg1 = model_entity_load,
        [RES_MODEL_ENTITY].save_stg0 = model_entity_save,
        [RES_MODEL_ENTITY].save_stg1 = save_model_entity,
        [RES_MODEL_ENTITY].free_stg0 = model_entity_free,
        [RES_MODEL_ENTITY].free_stg1 = free_model_entity
};

#define ref_task( _task )               ((_task)->n_ref ++)
#define deref_task( _task ) \
{ \
        if ( (_task)->n_ref == 0 ) { \
                log_critical_err_dbg ( "over get task resource \n\t%s\n", (_task)->filename ); \
        } else { \
                (_task)->n_ref --; \
        } \
}

struct res_loader *create_res_loader ( void )
{
        struct res_loader *loader = alloc_fix ( sizeof *loader, 1 );
        memset ( loader, 0, sizeof *loader );
        loader->task = alloc_fix ( sizeof *loader->task, MAX_RESOURCE );
        int type;
        for ( type = 0; type < MAX_RESOURCE; type ++ ) {
                zero_obj ( &loader->task[type] );
                create_stager ( nullptr, 1024, &loader->task[type].content );
                alg_init ( llist, &loader->task[type].task, sizeof (struct res_task*), 0 );
                loader->task[type].n_task = 0;
        }
        thread_init_trap ( &loader->trap );
        loader->wgroup = thread_new_workgroup ( 1 );
        loader->is_valid = true;
        loader->pivot = 1;
        return loader;
}

void free_res_loader ( struct res_loader *loader )
{
        thread_trap_on_task ( &loader->trap );
        thread_free_workgroup ( loader->wgroup );
        loader->wgroup = nullptr;
        int type;
        for ( type = 0; type < MAX_RESOURCE; type ++ ) {
                free_stager ( &loader->task[type].content );
                alg_free ( llist, &loader->task[type].task );
                memset ( &loader->task[type], 0, sizeof *loader->task );
        }
        free_fix ( loader->task );
        loader->is_valid = false;
        thread_untrap_task ( &loader->trap );
}

void res_loader_flush ( struct res_loader *loader )
{
        thread_trap_on_task ( &loader->trap );
        thread_abandon_workgroup ( loader->wgroup );
        loader->wgroup = thread_new_workgroup ( 1 );
        int type;
        for ( type = 0; type < MAX_RESOURCE; type ++ ) {
                stager_flush ( &loader->task[type].content );
                alg_flush ( llist, &loader->task[type].task );
                loader->task[type].n_task = 0;
        }
        loader->is_valid = true;
        loader->pivot = SANITIZE_THRESHOLD;
        thread_untrap_task ( &loader->trap );
}

void res_loader_sanitize ( struct res_loader *loader )
{
        if ( loader->total_task > loader->pivot ) {
                thread_trap_on_task ( &loader->trap );
                /* @todo (davis#3#): release zero-referenced tasks */
                loader->pivot = loader->total_task*2;
                thread_untrap_task ( &loader->trap );
        }
}

static void init_res_task ( char *filename, enum RES_IDR type, struct res_loader *loader,
                            struct res_task *task )
{
        memset ( task, 0, sizeof *task );
        task->state = RES_STATE_LOADING;
        task->filename = alloc_fix ( 1, strlen ( filename ) + 1 );
        strcpy ( task->filename, filename );
        task->loader = loader;
        task->type = type;
        task->stg0_info = alloc_var ( sizeof (struct res_comp), 0 );
        task->stg1_inst = alloc_var ( sizeof (res_ptr_t), 0 );
        task->n_stg1 = 0;
        task->n_ref = 0;
}

static void free_res_task ( struct res_task *task )
{
        task->state = RES_STATE_EMPTY;
        task->loader = nullptr;
        free_fix ( task->filename );
        free_var ( task->stg0_info );
        free_var ( task->stg1_inst );
        memset ( task, 0, sizeof *task );
}

static void *async_run_loader ( void *info )
{
        struct res_task *task = info;
        if ( !g_res_ops[task->type].load_stg0 ( task->filename, task ) ) {
                log_severe_err_dbg ( "this resource \n\t%s\n of type %d failed the first-stage loading",
                                     task->filename, task->type );
                task->state = RES_STATE_FAILED;
                return nullptr;
        }
        if ( task->n_stg1 == 0 ) {
                log_normal_dbg ( "this resource \n\t%s\n of type %d is empty",
                                 task->filename, task->type );
                task->state = RES_STATE_EMPTY;
                return nullptr;
        }
        int i;
        for ( i = 0; i < task->n_stg1; i ++ ) {
                task->stg1_inst[i] =
                        g_res_ops[task->type].load_stg1 ( &task->stg0_info[i] );
                if ( task->stg1_inst[i] ) {
                        log_severe_err_dbg ( "this resource \n\t%s\n of type %d failed the second-stage loading of the %dth instance", task->filename, task->type, i );
                        task->state = RES_STATE_FAILED;
                }
        }
        task->state = task->state == RES_STATE_LOADING ?
                      RES_STATE_LOADED : task->state;
        ref_task ( task );
        return task;
}

struct res_task *res_loader_load ( char *filename, enum RES_IDR type, bool is_async,
                                   struct res_loader *loader )
{
        thread_trap_on_task ( &loader->trap );
        if ( !loader->is_valid ) {
                log_severe_err_dbg ( "loader has been released but this function is called" );
                return nullptr;
        }

        entry_rec_t *entry = create_entry_record ( &loader->task[type].content );
        stager_enter_root ( filename, entry );
        enum RES_ERROR error = stager_set_layer_first ( entry );
        struct res_task *task = nullptr;

        if ( error == RES_ERR_INVALID ) {
                /* make new task */
                task = alloc_fix ( sizeof (struct res_task), 1 );
retry_res:
                init_res_task ( filename, type, loader, task );
                stager_push_data ( &task, sizeof(struct res_task*), entry );
                alg_push_back ( llist, &loader->task[type].task, &task );

                if ( is_async ) {
                        task->thrtask = thread_run_task(get_function_name(), async_run_loader, task, loader->wgroup);
                } else {
                        async_run_loader ( task );
                }
                loader->task[type].n_task ++;
                loader->total_task ++;
        } else {
                /* return existing task */
                task = stager_pop_data_c ( struct res_task*, entry );
                if ( task->state == RES_STATE_FAILED ||
                     task->state == RES_STATE_EMPTY ) {
                        free_res_task ( task );
                        goto retry_res;
                } else if ( task->state == RES_STATE_LOADED ) {
                        ref_task ( task );
                }
        }

        free_entry_record ( entry );
        thread_untrap_task ( &loader->trap );
        return task;
}

static void *async_run_saver ( void *info )
{
        struct res_task *task = info;
        if ( !g_res_ops[task->type].save_stg0 ( task->stg1_inst, task ) ) {
                log_severe_err_dbg ( "this resource \n\t%s\n of type %d failed the first-stage saving",
                                     task->filename, task->type );
                task->state = RES_STATE_FAILED;
                return nullptr;
        }
        if ( task->n_stg1 == 0 ) {
                log_normal_dbg ( "this resource \n\t%s\n of type %d is empty",
                                 task->filename, task->type );
                task->state = RES_STATE_EMPTY;
                return nullptr;
        }
        int i;
        for ( i = 0; i < task->n_stg1; i ++ ) {
                if ( !g_res_ops[task->type].save_stg1 (
                             task->filename, &task->stg0_info[i] ) ) {
                        log_severe_err_dbg ( "this resource \n\t%s\n of type %d failed the second-stage loading of the %dth instance", task->filename, task->type, i );
                        task->state = RES_STATE_FAILED;
                }
        }
        task->state = task->state == RES_STATE_LOADING ?
                      RES_STATE_LOADED : task->state;
        ref_task ( task );
        return task;
}

struct res_task *res_loader_save ( char *filename, res_ptr_t res_ptr, enum RES_IDR type,
                                   bool is_async, struct res_loader *loader )
{
        thread_trap_on_task ( &loader->trap );
        if ( !loader->is_valid ) {
                log_severe_err_dbg ( "loader has been released but this function is called" );
                return nullptr;
        }

        struct res_task *task = alloc_fix ( sizeof (struct res_task), 1 );
        init_res_task ( filename, type, loader, task );

        if ( is_async ) {
                task->thrtask =
                        thread_run_task(get_function_name(), async_run_saver, task, loader->wgroup);
        } else {
                async_run_saver ( task );
        }
        loader->task[type].n_task ++;
        loader->total_task ++;

        thread_untrap_task ( &loader->trap );
        return task;
}

struct res_task *res_loader_pop ( enum RES_IDR type, struct res_loader *loader )
{
        struct res_task* task = nullptr;
        if ( alg_n ( llist, &loader->task[type].task ) > 0 ) {
                alg_iter(struct res_task*) iter;
                alg_last ( llist, &loader->task[type].task, iter );
                task = alg_access ( iter );
                alg_pop_back ( llist, &loader->task[type].task, iter );
        }
        res_loader_sanitize ( loader );
        return task;
}

enum RES_STATE res_loader_task_query ( struct res_task *task )
{
        return task->state;
}

res_ptr_t res_loader_task_get ( struct res_task *task, int *n_inst, enum RES_STATE *state )
{
        res_loader_sanitize ( task->loader );
retry_pop:
        *state = task->state;

        switch ( task->state ) {
        case RES_STATE_LOADING: {
                thread_sync_with_task ( task->thrtask );
                goto retry_pop;
        }
        case RES_STATE_FAILED:
        case RES_STATE_EMPTY: {
                return nullptr;
        }
        case RES_STATE_LOADED: {
                *n_inst = task->n_stg1;
                deref_task ( task );
                return task->stg1_inst;
        }
        default: {
                log_critical_err_dbg ( "unknown resource state generated, check memory error!" );
                return nullptr;
        }
        }
}

void res_loader_task_free ( struct res_task *task )
{
        free_res_task ( task );
}

struct res_comp *res_comp_alloc_unit ( struct res_task *task )
{
        task->stg0_info = alloc_add_var ( task->stg0_info, 1 );
        struct res_comp *comp = &task->stg0_info[task->n_stg1 ++];
        comp->loader = task->loader;
        memset ( comp, 0, sizeof *comp );
        return comp;
}

#define init_template( structname, _code ) \
{ \
        if ( !comp->structname ) { \
                comp->structname = \
                        alloc_var ( sizeof *comp->structname, 0 ); \
        } \
        typeof (comp->structname) inst = \
                &comp->structname[comp->n##structname ++]; \
        { \
                _code \
        } \
        memcpy ( inst->data, &inst, sizeof (inst) ); \
        return (untyped *) inst->data + sizeof (inst); \
}

void *res_comp_alloc_vertex ( int size, int count, struct res_comp *comp )
{
        init_template ( vertex,
                        inst->data = alloc_var ( 1, size*count + sizeof (inst) );
                        inst->elm_size = size;
                        inst->count = count;
                      )
}

void *res_comp_alloc_normal ( int size, int count, struct res_comp *comp )
{
        init_template ( normal,
                        inst->data = alloc_var ( 1, size*count + sizeof (inst) );
                        inst->elm_size = size;
                        inst->count = count;
                      )
}

void *res_comp_alloc_uv ( int size, int count, struct res_comp *comp )
{
        init_template ( uv,
                        inst->data = alloc_var ( 1, size*count + sizeof (inst) );
                        inst->elm_size = size;
                        inst->count = count;
                      )
}

void *res_comp_alloc_tangent ( int size, int count, struct res_comp *comp )
{
        init_template ( tangent,
                        inst->data = alloc_var ( 1, size*count + sizeof (inst) );
                        inst->elm_size = size;
                        inst->count = count;
                      )
}

void *res_comp_alloc_face ( int num_vertex, int count, struct res_comp *comp )
{
        init_template ( face,
                        inst->data = alloc_var ( 1,
                                        num_vertex*sizeof(int)*count + sizeof (inst) );
                        inst->n_vert = num_vertex;
                        inst->count = count;
                      )
}

void *res_comp_alloc_chars ( char *name, int count, struct res_comp *comp )
{
        init_template ( chars,
                        inst->name = alloc_fix ( sizeof(char), strlen (name) + 1 );
                        strcpy ( inst->name, name );
                        inst->data = alloc_var ( 1,
                                        sizeof(char)*count + sizeof (inst) );
                        inst->count = count;
                      )
}

void *res_comp_alloc_ints ( char *name, int count, struct res_comp *comp )
{
        init_template ( ints,
                        inst->name = alloc_fix ( sizeof(char), strlen (name) + 1 );
                        strcpy ( inst->name, name );
                        inst->data = alloc_var ( 1,
                                        sizeof(int)*count + sizeof (inst) );
                        inst->count = count;
                      )
}

void *res_comp_alloc_floats ( char *name, int count, struct res_comp *comp )
{
        init_template ( floats,
                        inst->name = alloc_fix ( sizeof(char), strlen (name) + 1 );
                        strcpy ( inst->name, name );
                        inst->data = alloc_var ( 1,
                                        sizeof(float)*count + sizeof (inst) );
                        inst->count = count;
                      )
}

void *res_comp_alloc_refptrs ( char *name, int count, struct res_comp *comp )
{
        init_template ( refptrs,
                        inst->name = alloc_fix ( sizeof(char), strlen (name) + 1 );
                        strcpy ( inst->name, name );
                        inst->data = alloc_var ( 1,
                                        sizeof(void*)*count + sizeof (inst) );
                        inst->count = count;
                      )
}

void *res_comp_alloc_image ( char *name, int w, int h, int size, struct res_comp *comp )
{
        init_template ( image,
                        inst->name = alloc_fix ( sizeof(char), strlen (name) + 1 );
                        strcpy ( inst->name, name );
                        inst->x = w;
                        inst->y = h;
                        inst->elm_size = size;
                        inst->data = alloc_var ( 1,
                                        size*w*h + sizeof (inst) );
                      )
}

void *res_comp_shrink ( void *ptr, int real_count )
{
        void *head = (untyped *) ptr - sizeof (void*);
        void *data;
        memcpy ( &data, &head, sizeof (void*) );
        alloc_expand_var ( data, real_count );
        return (untyped *) data + sizeof (void*);
}
