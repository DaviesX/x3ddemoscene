#ifndef RENDERABLECONTAINERIMPL_H_INCLUDED
#define RENDERABLECONTAINERIMPL_H_INCLUDED

/* @todo (davis#1#): don't need renderable dependency later... */
#include <container/linkedlist.h>
#include <x3d/renderable.h>

struct rdacontainer;


struct rdacontainer_ops {
        void (*update) (struct rdacontainer* self);
        void (*free) (struct rdacontainer* self);
        void (*add) (struct rdacontainer* self, enum RENDERABLE_IDR type, struct rda_instance* inst);
        void (*remove) (struct rdacontainer* self, enum RENDERABLE_IDR type, struct rda_instance* inst);
        void (*clear) (struct rdacontainer* self);
        struct rda_instance** (*frustum_find) (struct rdacontainer* self, enum RENDERABLE_IDR type, struct frustum3d* f);
};

/* Base class */
struct rdacontainer {
        struct rdacontainer_ops                 ops;
        d_alg_llist(struct rda_instance*)       inst_store;
        int                                     num_instance;
        int                                     num_renderable;
};

/* linear aggregate */
struct rdacontainer_linear {
        struct rdacontainer     _parent;
        struct rda_instance**   insts[MAX_RENDERABLE_TYPE];
};

struct rdacontainer_bvh {
        struct rdacontainer        _parent;
};

struct rdacontainer_grid {
        struct rdacontainer        _parent;
};


struct rdacontainer*    rdacontainer_create(enum RAG_IDR type);
void                    rdacontainer_free(struct rdacontainer* cont);
void                    rdacontainer_update(struct rdacontainer* cont);
void                    rdacontainer_clear(struct rdacontainer* cont);
void                    rdacontainer_add_instance(struct rdacontainer* cont, enum RENDERABLE_IDR type, struct rda_instance* inst);
void                    rdacontainer_remove_instance(struct rdacontainer* cont, enum RENDERABLE_IDR type, struct rda_instance* inst);
struct rda_instance**   rdacontainer_frustum_find(struct rdacontainer* cont, enum RENDERABLE_IDR type, struct frustum3d* f);
int                     rdacontainer_get_instance_count(struct rdacontainer* cont);
int                     rdacontainer_get_renderable_count(struct rdacontainer* cont);
bool                    rdacontainer_has_instance(struct rdacontainer* cont, struct rda_instance* inst);


#endif // RENDERABLECONTAINERIMPL_H_INCLUDED
