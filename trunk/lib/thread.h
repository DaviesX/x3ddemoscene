#ifndef X3DTHREAD_H_INCLUDED
#define X3DTHREAD_H_INCLUDED

#include <x3d/common.h>
#include <algorithm.h>


struct thr_task;
struct work_group;
struct thr_trap;
typedef void *(*f_Thread_Handler) ( void *info );

/*
 * structures
 */

struct thr_trap {
#ifdef X3D_COMPILER_GCC
        pthread_cond_t data;
        pthread_mutex_t lock;
        sem_t counter;
#endif
};


/*
 * functions' declaration
 */
void init_thread_lib ( void );
void free_thread_lib ( void );
struct work_group *thr_new_workgroup ( int n_parallel );
void thr_free_workgroup ( struct work_group *group );
void thr_abandon_workgroup ( struct work_group *group );
bool thr_is_workgroup_complete ( struct work_group *group );
struct thr_task *thr_run_task (
        f_Thread_Handler task_func, void *info, struct work_group *group );
void free_thr_task ( struct thr_task *task );
void thr_task_abort ( struct thr_task *task );
bool thr_is_task_complete ( struct thr_task *task );
bool thr_task_quit_signal ( struct thr_task *task );
void thr_sync_with_task ( struct thr_task *task );
void thr_block_task ( struct thr_task *task );
void thr_unblock_task ( struct thr_task *task );
void thr_init_trap ( struct thr_trap *trap );
void thr_trap_on_data ( struct thr_trap *data, struct thr_trap *trap );
void thr_untrap_data ( struct thr_trap *data );
void thr_trap_on_task ( struct thr_trap *trap );
void thr_untrap_task ( struct thr_trap *trap );


#endif // X3DTHREAD_H_INCLUDED
