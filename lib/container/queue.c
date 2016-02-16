#include <container/queue.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>


// Test Cases:
// variable_memory_test0
__dlexport void __callback                  queue_correctness_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  queue_correctness_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   queue_correctness_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  queue_correctness_test(struct alg_var_set* envir)
{
        bool failed = false;
        
        queue_templ(int) queue;
        queue_init(&queue, 10);
        
        const int n = 10;
        int array[n];
        int i;
        for (i = 0; i < n; i ++) {
                array[i] = rand() % n;
        }
        
        log_normal("testing push back");
        for (i = 0; i < n; i ++) {
                queue_push_back(&queue, array[i]);
        }
        i = 0;
        queue_iter_templ(int) iterator;
        queue_iter_begin(&iterator, &queue);
        while (queue_iter_has_value(&iterator, &queue)) {
                int e = queue_iter_deref(&iterator, &queue);
                if (e != array[i]) {
                        log_severe_err("%d != %d", e, array[i]);
                        failed = true;
                } else {
                        log_normal("%d", e);
                }
                i ++;
                queue_iter_next(&iterator, &queue);
        }
        if (queue_size(&queue) != n) {
                log_severe_err("size != %d after pushing all", n);
                failed = true;
        }
        queue_flush(&queue);
        if (queue_size(&queue) != 0) {
                log_severe_err("size != 0 after flush");
                failed = true;
        }
        queue_free(&queue); 
        
        if (!failed) {
                log_normal("queue passed all tests");
        } else {
                log_normal("queue failed in some tests");
        }
}