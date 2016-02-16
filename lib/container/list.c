#include <container/list.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>


// Test Cases:
// variable_memory_test0
__dlexport void __callback                  list_correctness_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  list_correctness_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   list_correctness_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}

typedef list_iter_templ(int)    list_iter_t;
typedef list_templ(int)         list_t;

static list_iter_t get_random_iterator(list_t* list, int length)
{
        list_iter_t it;
        list_iter_begin(&it, list);
        while (true) {
                while (list_iter_has_value(&it, list)) {
                        if (rand()%length == length/2) {
                                // mutate and return
                                return it;
                        } else {
                                list_iter_next(&it, list);
                        }
                }
        }
}

__dlexport void __callback                  list_correctness_test(struct alg_var_set* envir)
{
        bool failed = false;
        
        list_t list;
        list_init(&list, 1);
        
        const int n = 10;
        int array[n];
        int i;
        for (i = 0; i < n; i ++) {
                array[i] = rand() % n;
        }
        
        // Test push back
        log_normal("testing push back");
        for (i = 0; i < n; i ++) {
                list_push_back(&list, array[i]);
        }
        i = 0;
        list_t iterator;
        list_iter_begin(&iterator, &list);
        while (list_iter_has_value(&iterator, &list)) {
                int e = list_iter_deref(&iterator, &list);
                if (e != array[i]) {
                        log_severe_err("%d != %d", e, array[i]);
                        failed = true;
                } else {
                        log_normal("%d", e);
                }
                i ++;
                list_iter_next(&iterator, &list);
        }
        if (list_size(&list) != n) {
                log_severe_err("size != %d after pushing all", n);
                failed = true;
        }
        list_flush(&list);
        if (list_size(&list) != 0) {
                log_severe_err("size != 0 after flush");
                failed = true;
        }
        // Test push front
//        log_normal("testing push front");
//        for (i = 0; i < n; i ++) {
//                list_push_front(&list, array[i]);
//        }
//        i = 0;
//        list_iter_begin(&iterator, &list);
//        while (list_iter_has_value(&iterator, &list)) {
//                int e = list_iter_deref(&iterator, &list);
//                if (e != array[n - i - 1]) {
//                        log_severe_err("%d != %d", e, array[n - i - 1]);
//                        failed = true;
//                } else {
//                        log_normal("%d", e);
//                }
//                i ++;
//                list_iter_next(&iterator, &list);
//        }
        if (list_size(&list) != n) {
                log_severe_err("size != %d after pushing all", n);
                failed = true;
        }
        list_flush(&list);
        if (list_size(&list) != 0) {
                log_severe_err("size != 0 after flush");
                failed = true;
        }
        // Test random insertion
        list_iter_begin(&iterator, &list);
        while (list_iter_has_value(&iterator, &list)) {
                int e = list_iter_deref(&iterator, &list);
                if (e != array[i]) {
                        log_severe_err("%d != %d", e, array[i]);
                        failed = true;
                } else {
                        log_normal("%d", e);
                }
                i ++;
                list_iter_next(&iterator, &list);
        }
        // Test removal
        list_free(&list); 
        
        if (!failed) {
                log_normal("list passed all tests");
        } else {
                log_normal("list failed in some tests");
        }
}