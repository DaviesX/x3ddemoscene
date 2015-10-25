#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <x3d/common.h>


struct thread_task;
struct work_group;
struct thread_trap;
typedef void *(*f_Thread_Handler) ( void *info );

/*
 * structures
 */

struct thread_trap {
#ifdef X3D_PLATFORM_POSIX
        pthread_cond_t  data;
        pthread_mutex_t lock;
        sem_t           counter;
        pthread_t       tids[100];
#endif
        int             verifier[100];
        int             count;
};


/*
 * functions' declaration
 */
void                    thread_lib_init();
void                    thread_lib_free();
struct work_group*      thread_new_workgroup(int n_parallel);
void                    thread_free_workgroup(struct work_group* group);
void                    thread_abandon_workgroup(struct work_group* group);
bool                    thread_is_workgroup_complete(struct work_group* group);
struct thread_task*     thread_run_task(const char* name, f_Thread_Handler task_func, void* info, struct work_group* group);
void                    thread_task_free(struct thread_task* task);
void                    thread_task_abort(struct thread_task* task);
bool                    thread_is_task_complete(struct thread_task* task);
bool                    thread_task_quit_signal(struct thread_task* task);
void                    thread_sync_with_task(struct thread_task* task);
void                    thread_block_task(struct thread_task* task);
void                    thread_unblock_task(struct thread_task* task);
void                    thread_init_trap(struct thread_trap* trap);
void                    thread_trap_on_data(struct thread_trap* data, struct thread_trap* trap);
void                    thread_untrap_data(struct thread_trap* data);
void                    thread_trap_on_task(struct thread_trap* trap);
void                    thread_untrap_task(struct thread_trap* trap);
void                    thread_task_idle(int milisec);


#endif // THREAD_H_INCLUDED
