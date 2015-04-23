/* thread.c: parallel task management utilities all go here */
#include <container/linkedlist.h>
#include <system/allocator.h>
#include <system/thread.h>

#define INIT_GROUP_COUNT                1
#define MAX_GROUP_COUNT                 5

#define LOCK_SHARED                     0       /* shared lock in a workgroup */
#define LOCK_TASK                       1       /* lock for task queuing */
#define LOCK_THREAD                     2       /* lock for thread arrangment in a workgroup */
#define LOCK_OCCUPIED                   3       /* lock for occupied task */
#define LOCK_THREAD_POOL                4       /* lock for global thread pool */

struct thread_ctx;
struct work_group;

struct thr_task {
        char*                           task_name;
        bool                            quit_signal;
        bool                            is_waiting;
        bool                            is_processed;
        alg_iter(struct thr_task*)      addr_id;
        struct thr_trap                 sync_trap;
        f_Thread_Handler                func;
        void*                           info;
        struct work_group*              binded_group;
        struct thread_ctx*              binded_thr;
};

struct thread_ctx {
        bool                            is_active;
        alg_iter(struct thread_ctx*)    addr_id;
        struct work_group*              group;
        void *(*handler) ( void *task );
#ifdef X3D_COMPILER_GCC
        pthread_t handle;
#endif
};

struct work_group {
        bool is_active;
        struct alg_llist tasks;
        struct alg_llist threads;
        int n_tasks;
        int n_threads;
        struct thr_trap idler;
        struct thr_trap lock[3];
        struct work_group *next;        /* using customized linked list for workgroup-reuse */
        struct work_group *prev;
};

struct thread_pool {
        struct work_group *grouped_head;
        struct work_group *grouped;
        struct work_group *occupied;
        struct thr_trap self_lock;
};

static long g_ncores = 0;       /* platform information */
static struct thread_pool g_thrpool;


static bool retrieve_task ( struct work_group *group, struct thread_ctx *thread,
                            struct thr_task **task );
static void *thread_handler ( void *info );

/* work group operations */
static void                     workgroup_make_inactive ( struct work_group *group );
static void                     workgroup_make_active ( int n_thread, struct work_group *group );
static struct work_group*       create_workgroup ( void );
static void                     free_workgroup ( struct work_group *group );

static struct thread_ctx*       workgroup_addthread ( struct work_group *group );
static void                     workgroup_removethread ( struct thread_ctx *thread, struct work_group *group );
static void                     workgroup_removeallthread ( struct work_group *group );

static struct work_group*       workgroup_new ( int n_thread, struct work_group *group );
static void                     workgroup_remove ( struct work_group *group );
static void                     workgroup_abandon ( struct work_group *group );

static void                     workgroup_addtask ( struct thr_task *task, struct work_group *group );
static void                     workgroup_removetask ( struct thr_task *task, struct work_group *group );
static void                     workgroup_removealltask ( struct work_group *group );

/* task operations */
static struct thr_task*         task_create(const char* name, struct work_group* binded_group,
                                            f_Thread_Handler task_func, void* task_data);
static void                     task_free(struct thr_task* self);
static void                     task_terminate(struct thr_task* self);
static void                     task_synchronize(struct thr_task* self);

/* platform specifc APIs */
static void init_trap ( struct thr_trap *trap );
static void trap_on_thread ( struct thr_trap *trap );
static void remove_thread_trap ( struct thr_trap *trap );
static void trap_on_data ( struct thr_trap *data, struct thr_trap *trap );
static void remove_data_trap ( struct thr_trap *data );
static void trap_on_counter ( struct thr_trap *trap );
static void remove_counter_trap ( struct thr_trap *trap );
static void generate_thread ( struct thread_ctx *thread );
static void current_thread_exit ( void );
static void join_thread ( struct thread_ctx *thread );
static long get_cpu_corecount ( void );

#ifdef X3D_PLATFORM_POSIX
const pthread_mutex_t c_mutex_init = PTHREAD_MUTEX_INITIALIZER;
const pthread_cond_t c_cond_init = PTHREAD_COND_INITIALIZER;

static void init_trap ( struct thr_trap *trap )
{
        sem_init ( &trap->counter, 0, 0 );
        trap->lock = c_mutex_init;
        trap->data = c_cond_init;
}

static void trap_on_thread ( struct thr_trap *trap )
{
        pthread_mutex_lock ( &trap->lock );
}

static void remove_thread_trap ( struct thr_trap *trap )
{
        pthread_mutex_unlock ( &trap->lock );
}

static void trap_on_data ( struct thr_trap *data, struct thr_trap *trap )
{
        pthread_cond_wait ( &data->data, &trap->lock );
}

static void remove_data_trap ( struct thr_trap *data )
{
        pthread_cond_signal ( &data->data );
}

static void trap_on_counter ( struct thr_trap *trap )
{
        sem_wait ( &trap->counter );
}

static void remove_counter_trap ( struct thr_trap *trap )
{
        sem_post ( &trap->counter );
}

static void generate_thread ( struct thread_ctx *thread )
{
        pthread_create ( &thread->handle, nullptr, thread->handler, thread );
}

static void current_thread_exit ( void )
{
        pthread_exit ( nullptr );
}

static void join_thread ( struct thread_ctx *thread )
{
        pthread_join ( thread->handle, nullptr );
}

static long get_cpu_corecount ( void )
{
        return sysconf ( _SC_NPROCESSORS_ONLN );
}

static void idle ( int milisec )
{
        usleep ( milisec*1000 );
}

#endif // X3D_PLATFORM_POSIX


static bool retrieve_task ( struct work_group *group, struct thread_ctx *thread,
                            struct thr_task **task )
{
        trap_on_thread ( &group->lock[LOCK_TASK] );
        if ( *task != nullptr ) {
                (*task)->is_processed = true;
                remove_data_trap ( &(*task)->sync_trap );
                group->n_tasks --;
        }
        remove_thread_trap ( &group->lock[LOCK_TASK] );

        /* idle current thread when all tasks were executed
         * awake right after new tasks were fed in to the task queue */
        trap_on_counter ( &group->idler );

        trap_on_thread ( &group->lock[LOCK_TASK] );
        bool active = group->is_active && thread->is_active;
        if ( active ) {
                /* fetch new task and mark the new task as not being processed */
                alg_iter(struct thr_task*) fetch;
                alg_last ( llist, &group->tasks, fetch );
                *task = alg_access ( fetch );
                alg_pop_back ( llist, &group->tasks, fetch );
                (*task)->is_waiting = false;
                (*task)->binded_thr = thread;
        }
        remove_thread_trap ( &group->lock[LOCK_TASK] );
        return active;
}

static void *thread_handler ( void *info )
{
        struct thread_ctx *thread = info;
        struct work_group *group = thread->group;
        struct thr_task *task = nullptr;
        /* traverse and execute the entire task queue */
        while ( retrieve_task ( group, thread, &task ) ) {
                task->func ( task->info );
        }
        current_thread_exit ();
        return nullptr;
}

static void workgroup_make_inactive ( struct work_group *group )
{
        group->is_active = false;
        init_trap ( &group->idler );
        init_trap ( &group->lock[LOCK_SHARED] );
        init_trap ( &group->lock[LOCK_TASK] );
        init_trap ( &group->lock[LOCK_THREAD] );
        group->n_tasks = 0;
        group->n_threads = 0;
        alg_init ( llist, &group->tasks, sizeof(struct thr_task*), 0 );
        alg_init ( llist, &group->threads, sizeof(struct thread_ctx*), 0 );
}

static void workgroup_make_active ( int n_thread, struct work_group *group )
{
        group->is_active = true;
        int i;
        for ( i = 0; i < n_thread; i ++ )
                workgroup_addthread(group);
}

static void workgroup_free_active ( struct work_group *group )
{
        group->is_active = false;
        workgroup_removealltask(group);
        workgroup_removeallthread(group);
        init_trap(&group->idler);
        init_trap(&group->lock[0]);
        init_trap(&group->lock[1]);
        group->n_tasks = 0;
        group->n_threads = 0;
        alg_llist_flush(&group->tasks);
        alg_llist_flush(&group->threads);
}

static void workgroup_free_inactive ( struct work_group *group )
{
        group->is_active = false;
        free_alg_llist ( &group->tasks );
        free_alg_llist ( &group->threads );
}

static struct work_group *create_workgroup ( void )
{
        struct work_group *group = alloc_fix ( sizeof *group, 1 );
        group->next = nullptr;
        group->prev = nullptr;
        workgroup_make_inactive ( group );
        return group;
}

static void free_workgroup ( struct work_group *group )
{
        while(group->next != nullptr) {
                group = group->next;
                workgroup_remove(group->prev);
        }
        workgroup_remove(group);
}

static struct thread_ctx *workgroup_addthread ( struct work_group *group )
{
        /* setup information */
        struct thread_ctx* thread = alloc_obj(thread);
        thread->is_active = true;
        thread->group = group;
        thread->handler = thread_handler;

        /* add into the list */
        trap_on_thread ( &group->lock[LOCK_THREAD] );
        alg_iter(struct thread_ctx*)    iter;
        alg_push_back ( llist, &group->threads, thread );
        alg_last ( llist, &group->threads, iter );
        thread->addr_id = iter;
        ((struct thread_ctx **) thread->addr_id)[0] = thread;
        group->n_threads ++;
        remove_thread_trap ( &group->lock[LOCK_THREAD] );

        /* run the thread */
        generate_thread ( thread );
        return thread;
}

static void workgroup_removethread ( struct thread_ctx *thread, struct work_group *group )
{
        /* notify the termination of the thread,
         * then join and wait for its exit */
        thread->is_active = false;
        join_thread ( thread );

        /* remove it from the thread list */
        trap_on_thread ( &group->lock[LOCK_THREAD] );
        alg_remove ( llist, &group->threads, thread->addr_id );
        ((struct thread_ctx **) thread->addr_id)[0] = nullptr;
        remove_thread_trap ( &group->lock[LOCK_THREAD] );

        free_fix ( thread );
}

static void workgroup_removeallthread ( struct work_group *group )
{
        trap_on_thread(&group->lock[LOCK_THREAD]);
        alg_iter(struct thread_ctx*)    curr_pos;
        alg_iter(struct thread_ctx*)    end;
        alg_first(llist, &group->threads, curr_pos);
        alg_null(llist, &group->threads, end);

        while ( curr_pos != end ) {
                struct thread_ctx *thread = alg_access(curr_pos);

                /* notify the termination of the thread,
                 * then join and wait for its exit */
                thread->is_active = false;
                /* untrap counters */
                int i;
                for (i = 0; i < group->n_threads; i ++) {
                        remove_counter_trap(&group->idler);
                }
                join_thread(thread);

                /* remove it from the thread list */
                alg_remove(llist, &group->threads, thread->addr_id);
                alg_access(thread->addr_id) = nullptr;

                free_fix(thread);
                /* get next thread */
                alg_next(llist, &group->threads, curr_pos);
        }
        remove_thread_trap(&group->lock[LOCK_THREAD]);
}

static struct work_group *workgroup_new ( int n_thread, struct work_group *group )
{
        if ( group->next == nullptr ) {
                group->next = alloc_obj(group->next);
                group->next->prev = group;
                group->next->next = nullptr;
                workgroup_make_inactive ( group->next );
        }
        if ( !group->is_active ) {
                workgroup_make_active ( n_thread, group );
        }
        return group;
}

static void workgroup_remove ( struct work_group *group )
{
        if (group == nullptr)
                return ;
        workgroup_free_active ( group );
        workgroup_free_inactive ( group );

        if ( group->prev != nullptr )
                group->prev->next = group->next;

        if ( group->next != nullptr )
                group->next->prev = group->prev;

        free_fix ( group );
}

static void workgroup_abandon ( struct work_group *group )
{
        workgroup_removealltask ( group );

        if ( group->prev != nullptr )
                group->prev->next = group->next;

        if ( group->next != nullptr )
                group->next->prev = group->prev;

        /* forward to the end and append the currently abandoned workgroup */
        struct work_group *curr = group;
        while ( group->next != nullptr )
                group = group->next;
        group->next = curr;
        curr->prev = group;
        curr->next = nullptr;
}

static void workgroup_addtask ( struct thr_task *task, struct work_group *group )
{
        trap_on_thread ( &group->lock[LOCK_TASK] );
        alg_push_back ( llist, &group->tasks, task );
        alg_last ( llist, &group->tasks, task->addr_id );
        group->n_tasks ++;
        remove_counter_trap ( &group->idler );
        remove_thread_trap ( &group->lock[LOCK_TASK] );
}

static void workgroup_removetask ( struct thr_task *task, struct work_group *group )
{
        trap_on_thread(&group->lock[LOCK_TASK]);
        if (task->is_waiting) {
                /* the task is still sitting in the queue, just remove it directly */
                alg_llist_remove(task->addr_id, &group->tasks);
                group->n_tasks --;
        } else if (task->is_processed) {
                /* the task is being processed, nothing have to do */
        } else {
                /* the task is being processing, try to notify and wait for its termination
                 * and after that, it then can safely be removed */
                task->quit_signal = true;
                trap_on_data(
                        &task->sync_trap,
                        &task->binded_group->lock[LOCK_TASK]);
                trap_on_thread(&group->lock[LOCK_TASK]);
        }
        task_free(task);
        remove_thread_trap(&group->lock[LOCK_TASK]);
}

static void workgroup_removealltask ( struct work_group *group )
{
        trap_on_thread ( &group->lock[LOCK_TASK]);
        alg_iter(struct thr_task*)      curr_pos;
        alg_iter(struct thr_task*)      end;
        alg_first(llist, &group->tasks, curr_pos);
        alg_null(llist, &group->tasks, end);

        while ( curr_pos != end ) {
                struct thr_task *task = alg_access(curr_pos);

                if (task->is_waiting) {
                        /* the task is still sitting in the queue, just remove it directly */
                        alg_llist_remove(task->addr_id, &group->tasks);
                        group->n_tasks --;
                } else if(task->is_processed) {
                        /* the task is being processed, nothing have to do */
                } else {
                        /* the task is being processing, try to notify and wait for its termination
                         * and after that, it then can safely be removed */
                        task->quit_signal = true;
                        trap_on_data(
                                &task->sync_trap,
                                &task->binded_group->lock[LOCK_TASK]);
                        trap_on_thread (&group->lock[LOCK_TASK]);
                }
                task_free(task);
                alg_next(llist, &group->tasks, curr_pos);
        }
        remove_thread_trap(&group->lock[LOCK_TASK]);
}

static struct thr_task* task_create(const char* name, struct work_group* binded_group,
                                    f_Thread_Handler task_func, void* task_data)
{
        struct thr_task* self   = alloc_obj(self);
        self->task_name         = alg_alloc_string(name);
        self->binded_group      = binded_group;
        self->addr_id           = nullptr;
        self->binded_thr        = nullptr;
        self->func              = task_func;
        self->info              = task_data;
        self->is_processed      = false;
        self->is_waiting        = true;
        self->quit_signal       = false;
        init_trap(&self->sync_trap);
        return self;
}

static void task_free(struct thr_task* self)
{
        free_fix(self->task_name);
        zero_obj(self);
        free_fix(self);
}

static void task_terminate(struct thr_task* self)
{
        self->quit_signal = true;
}

static void task_synchronize(struct thr_task* self)
{
        trap_on_thread ( &(self)->binded_group->lock[LOCK_TASK] );
        if ( self != nullptr && !(self)->is_processed ) {
                trap_on_data (
                        &(self)->sync_trap,
                        &(self)->binded_group->lock[LOCK_TASK] );
        }
        remove_thread_trap ( &(self)->binded_group->lock[LOCK_TASK] );
}

void thread_lib_init()
{
        memset ( &g_thrpool, 0, sizeof g_thrpool );
        g_ncores = get_cpu_corecount ();

        /* initialize the lock and playaround LOL */
        init_trap ( &g_thrpool.self_lock );
        trap_on_thread ( &g_thrpool.self_lock );

        /* pre-create certain amount of workgroups on initialization */
        g_thrpool.grouped_head = create_workgroup ();
        g_thrpool.grouped = g_thrpool.grouped_head;
        int i;
        for ( i = 0; i < INIT_GROUP_COUNT; i ++ ) {
                workgroup_new(g_ncores, g_thrpool.grouped);
                g_thrpool.grouped = g_thrpool.grouped->next;
        }
        g_thrpool.grouped = g_thrpool.grouped_head;

        /* for occupied tasks, threads are to be created on the fly */
        g_thrpool.occupied = create_workgroup ();
        g_thrpool.occupied = workgroup_new ( 0, g_thrpool.occupied );

        remove_thread_trap ( &g_thrpool.self_lock );
}

void thread_lib_free()
{
        trap_on_thread(&g_thrpool.self_lock);
        if (g_thrpool.grouped_head)
                free_workgroup(g_thrpool.grouped_head);
        if (g_thrpool.occupied)
                free_workgroup(g_thrpool.occupied);
        zero_obj(&g_thrpool);
        g_ncores = -1;
        remove_thread_trap(&g_thrpool.self_lock);
}

struct work_group *thr_new_workgroup ( int n_parallel )
{
        trap_on_thread ( &g_thrpool.self_lock );
        n_parallel = (n_parallel > 0) ? n_parallel : g_ncores;
        struct work_group *new_group =
                workgroup_new ( n_parallel, g_thrpool.grouped );
        g_thrpool.grouped = g_thrpool.grouped->next;
        remove_thread_trap ( &g_thrpool.self_lock );
        return new_group;
}

void thr_free_workgroup ( struct work_group *group )
{
        trap_on_thread ( &g_thrpool.self_lock );
        workgroup_remove ( group );
        remove_thread_trap ( &g_thrpool.self_lock );
}

void thr_abandon_workgroup ( struct work_group *group )
{
        trap_on_thread ( &g_thrpool.self_lock );
        workgroup_abandon ( group );
        remove_thread_trap ( &g_thrpool.self_lock );
}

bool thr_is_workgroup_complete ( struct work_group *group )
{
        return group->n_tasks == 0;
}

struct thr_task* thr_run_task(const char* name, f_Thread_Handler task_func, void *info, struct work_group *group)
{
        if (group == nullptr) {
                /* using the global work group */
                group = g_thrpool.occupied;
                workgroup_addthread(group);
        }
        struct thr_task* task = task_create(name, group, task_func, info);
        workgroup_addtask(task, group);
        return task;
}

void free_thr_task ( struct thr_task *task )
{
        workgroup_removetask ( task, task->binded_group );
}

void thr_task_abort(struct thr_task* task)
{
        task_terminate(task);
}

bool thr_is_task_complete ( struct thr_task *task )
{
        return task->is_processed;
}

bool thr_task_exit_signal ( struct thr_task *task )
{
        return task->quit_signal;
}

void thr_sync_with_task(struct thr_task* task)
{
        task_synchronize(task);
}

void thr_block_task ( struct thr_task *task )
{
        trap_on_thread ( &task->binded_group->lock[LOCK_SHARED] );
}

void thr_unblock_task ( struct thr_task *task )
{
        remove_thread_trap ( &task->binded_group->lock[LOCK_SHARED] );
}

void thr_init_trap ( struct thr_trap *trap )
{
        init_trap ( trap );
}

void thr_trap_on_data ( struct thr_trap *data, struct thr_trap *trap )
{
        trap_on_data ( data, trap );
}

void thr_untrap_data ( struct thr_trap *data )
{
        remove_data_trap ( data );
}

void thr_trap_on_task ( struct thr_trap *trap )
{
        trap_on_thread ( trap );
}

void thr_untrap_task ( struct thr_trap *trap )
{
        remove_thread_trap ( trap );
}

void thr_task_idle ( int milisec )
{
        idle ( milisec );
}
