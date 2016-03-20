#include <container/stack.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>


// Test Cases:
// variable_memory_test0
__dlexport void __callback                  stack_correctness_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  stack_correctness_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   stack_correctness_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  stack_correctness_test(struct alg_var_set* envir)
{
        bool failed = false;

        stack_templ(int) stack;
        stack_init(&stack, 10);

        const int n = 10;
        int array[n];
        int i;
        for (i = 0; i < n; i ++) {
                array[i] = rand() % n;
        }

        log_normal("testing push back");
        for (i = 0; i < n; i ++) {
                stack_push_back(&stack, array[i]);
        }
        i = 0;
        stack_iter_templ(int) iterator;
        stack_iter_begin(&iterator, &stack);
        while (stack_iter_has_value(&iterator, &stack)) {
                int e = stack_iter_deref(&iterator, &stack);
                if (e != array[i]) {
                        log_severe_err("%d != %d", e, array[i]);
                        failed = true;
                } else {
                        log_normal("%d", e);
                }
                i ++;
                stack_iter_next(&iterator, &stack);
        }
        if (stack_size(&stack) != n) {
                log_severe_err("size != %d after pushing all", n);
                failed = true;
        }
        stack_flush(&stack);
        if (stack_size(&stack) != 0) {
                log_severe_err("size != 0 after flush");
                failed = true;
        }
        stack_free(&stack);

        if (!failed) {
                log_normal("stack passed all tests");
        } else {
                log_normal("stack failed in some tests");
        }
}
