#include <container/deque.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>


// Test Cases:
// variable_memory_test0
__dlexport void __callback                  deque_correctness_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  deque_correctness_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   deque_correctness_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  deque_correctness_test(struct alg_var_set* envir)
{
        bool failed = false;

        deque_templ(int) deque;
        deque_init(&deque, 10);

        const int n = 10;
        int array[n];
        int i;
        for (i = 0; i < n; i ++) {
                array[i] = rand() % n;
        }

        // test push back
        log_normal("testing push back");
        for (i = 0; i < n; i ++) {
                deque_push_back(&deque, array[i]);
        }
        i = 0;
        deque_iter_templ(int) iterator;
        deque_iter_begin(&iterator, &deque);
        while (deque_iter_has_next(&iterator, &deque)) {
                int e = deque_iter_deref(&iterator, &deque);
                if (e != array[i]) {
                        log_severe_err("%d != %d", e, array[i]);
                        failed = true;
                } else {
                        log_normal("%d", e);
                }
                i ++;
                deque_iter_next(&iterator, &deque);
        }
        if (deque_size(&deque) != n) {
                log_severe_err("size != %d after pushing back", n);
                failed = true;
        }
        deque_flush(&deque);
        if (deque_size(&deque) != 0) {
                log_severe_err("size != 0 after flush");
                failed = true;
        }

        // test push front
        for (i = 0; i < n; i ++) {
                deque_push_front(&deque, array[i]);
        }
        i = 0;
        deque_iter_begin(&iterator, &deque);
        while (deque_iter_has_next(&iterator, &deque)) {
                int e = deque_iter_deref(&iterator, &deque);
                if (e != array[n - i - 1]) {
                        log_severe_err("%d != %d", e, array[n - i - 1]);
                        failed = true;
                } else {
                        log_normal("%d", e);
                }
                i ++;
                deque_iter_next(&iterator, &deque);
        }
        if (deque_size(&deque) != n) {
                log_severe_err("size != %d after pushing front", n);
                failed = true;
        }

        int front, back;
        deque_back(&deque, back);
        deque_front(&deque, front);
        if (front != array[n - 1] || back != array[0]) {
                log_severe_err("front/back accessor is incorrect: ");
                log_severe_err("%d", front != array[n - 1]);
                log_severe_err("%d", back != array[0]);
        }

        deque_free(&deque);

        if (!failed) {
                log_normal("deque passed all tests");
        } else {
                log_normal("deque failed in some tests");
        }
}
