#ifndef BALANCEDTREE_H_INCLUDED
#define BALANCEDTREE_H_INCLUDED

#include <container/container.h>

#pragma pack(4)

/* Provides a balance tree (currently unbalanced) container and
 * some common O(log n) operations below */
/*
 * structure
 */
struct alg_btree {
        void*   content;
        struct {
                int     node;
                int     left;
                int     right;
        } *     nodes;
        int*    node;
        int*    left;
        int*    right;
        int     size;
        int     n_head;
};
#pragma pack()

PseudoDef(void*         alg_btree_array(struct alg_btree* self);)
PseudoDef(int           alg_btree_n(struct alg_btree* self);)
PseudoDef(void          alg_btree_init(struct alg_btree* self, int elm_size, int multiplier, int init_count);)
PseudoDef(void          alg_btree_free(struct alg_btree* self);)
// PseudoDef(void          alg_btree_first(void** iterator, struct alg_btree* self);) /* currently not available */
// PseudoDef(void          alg_btree_next(void** iterator, struct alg_btree* self);); /*currently not available */
PseudoDef(void          alg_btree_find(struct alg_btree* self, uint32_t key, T data, void* iterator,
                                       int (*f_Compare) (T* data, void* elm));)
PseudoDef(void          alg_btree_insert(struct alg_btree* self, uint32_t key, T data, void* iterator,
                                         int (*f_Compare) (T* data, void* elm));)
PseudoDef(void          alg_btree_remove(struct alg_btree* self, void* iterator);)
PseudoDef(void          alg_btree_flush(struct alg_btree* self);)


/*
 * functions' definitions
 */
#define alg_btree_array(_self)          ((_self)->content)
#define alg_btree_n(_self)              ((_self)->size);

#define alg_btree_init(_self, _elm_size, _multiplier, _init_count)                   \
{                                                                                    \
        zero_obj(_self);                                                             \
        (_self)->content   = alloc_var(_elm_size, (_init_count));                    \
        (_self)->node      = alloc_var(sizeof(int), (_init_count) + (_multiplier));  \
        (_self)->left      = alloc_var(sizeof(int), (_init_count) + (_multiplier));  \
        (_self)->right     = alloc_var(sizeof(int), (_init_count) + (_multiplier));  \
        (_self)->n_head    = (_multiplier);                                          \
        (_self)->size      = 0;                                                      \
        int _i;                                                                      \
        for (_i = 0; _i < (_multiplier) + (_init_count); _i ++) {                    \
                (_self)->node[_i]       = NIL_POSITION;                              \
                (_self)->left[_i]       = NIL_POSITION;                              \
                (_self)->right[_i]      = NIL_POSITION;                              \
        }                                                                            \
}

#define alg_btree_free(_self)                                                        \
{                                                                                    \
        free_var((_self)->content);                                                  \
        free_var((_self)->node);                                                     \
        free_var((_self)->left);                                                     \
        free_var((_self)->right);                                                    \
        (_self)->n_head    = 0;                                                      \
        (_self)->size      = 0;                                                      \
        zero_obj(_self);                                                             \
}

#define alg_btree_find(_self, _key, _data, _iterator, f_Compare)                     \
{                                                                                    \
        (_iterator) = nullptr;                                                       \
                                                                                     \
        int root                = (_key)%(_self)->n_head;                            \
        int _i                  = (_self)->node[root];                               \
        typeof(*_iterator) arr  = (_self)->content;                                  \
        arr                     = arr[-(_self)->n_head];                             \
        while (_i != NIL_POSITION) {                                                 \
                int c = f_Compare(_data, &arr[_i]);                                  \
                if (c < 0) {                                                         \
                        /* left branch */                                            \
                        _i = (_self)->left[_i];                                      \
                } else if (c > 0) {                                                  \
                        /* right branch */                                           \
                        _i = (_self)->right[_i];                                     \
                } else {                                                             \
                        /* result found */                                           \
                        (_iterator) = &arr[_i];                                      \
                        break;                                                       \
                }                                                                    \
        }                                                                            \
}

#define alg_btree_insert(_self, _key, _data, _iterator, f_Compare)                   \
{                                                                                    \
        (_iterator) = nullptr;                                                       \
                                                                                     \
        (_self)->node           = alloc_var_add((_self)->node, 1);                   \
        (_self)->left           = alloc_var_add((_self)->left, 1);                   \
        (_self)->right          = alloc_var_add((_self)->right, 1);                  \
        (_self)->content        = alloc_var_add((_self)->content, 1);                \
        int new_node            = (_self)->size + (_self)->n_head;                   \
                                                                                     \
        int root                = (_key)%(_self)->n_head;                            \
        typeof(*_iterator) arr  = (_self)->content;                                  \
        arr                     = &arr[-(_self)->n_head];                            \
                                                                                     \
        if ((_self)->root_hash[root] == NIL_POSITION) {                              \
                /* allocate a new root */                                            \
                (_self)->root_hash[root] = new_node;                                 \
                (_self)->node[new_node]  = root;                                     \
                (_self)->left[new_node]  = NIL_POSITION;                             \
                (_self)->right[new_node] = NIL_POSITION;                             \
        } else {                                                                     \
                int _i = (_self)->root_hash[root];                                   \
                /* find insertion location */                                        \
                do {                                                                 \
                        int c = f_Compare(_data, &arr[_i]);                          \
                        if (c < 0) {                                                 \
                                /* left branch */                                    \
                                if ((_self)->left[_i] != NIL_POSITION)               \
                                        _i = (_self)->left[_i];                      \
                                else {                                               \
                                        /* insert to this spot */                    \
                                        (_self)->left[_i]        = new_node;         \
                                        (_self)->node[new_node]  = _i;               \
                                        (_self)->left[new_node]  = NIL_POSITION;     \
                                        (_self)->right[new_node] = NIL_POSITION;     \
                                        break;                                       \
                                }                                                    \
                        } else {                                                     \
                                /* right branch */                                   \
                                if ((_self)->right[_i] != NIL_POSITION)              \
                                        _i = (_self)->right[_i];                     \
                                else {                                               \
                                        /* insert to this spot */                    \
                                        (_self)->right[_i]       = new_node;         \
                                        (_self)->node[new_node]  = _i;               \
                                        (_self)->right[new_node] = NIL_POSITION;     \
                                        (_self)->right[new_node] = NIL_POSITION;     \
                                        break;                                       \
                                }                                                    \
                        }                                                            \
                } while (_i != NIL_POSITION);                                        \
        }                                                                            \
}

//PseudoDef(void          alg_btree_remove(struct alg_btree* self, void* iterator);)
//PseudoDef(void          alg_btree_flush(struct alg_btree* self);)



#endif // BALANCEDTREE_H_INCLUDED
