#ifndef MODEL_ENTITY_H_INCLUDED
#define MODEL_ENTITY_H_INCLUDED


#include <math/math.h>
#include <x3d/resloader.h>


enum MODEL_ENTITY_IDR {
        MODEL_ENTITY_STATIC = 1 << 1,
        MODEL_ENTITY_DYNAMIC = 1 << 2,
        MODEL_ENTITY_GLOBAL = 1 << 3,
        MODEL_ENTITY_RENDERABLE = 1 << 4,
        MODEL_ENTITY_KINEMATIC	= 1 << 5,
        MODEL_ENTITY_LOGICAL = 1 << 6
};


struct renderable;
struct dynamics;
struct logical;


struct model_entity {
        enum MODEL_ENTITY_IDR idr;
        struct box3d bound;
        struct renderable *renderable;
        struct dynamics *dynamics;
        struct logical *logical;
};

struct mesh_model {
        struct model_entity _entity;
        int *index;
        int n_index;
        struct point3d *vertex;
        int n_vertex;
};


/*
 * functions' declaration
 */
res_ptr_t model_entity_load ( struct res_comp *comp );
bool model_entity_save ( res_ptr_t ptr, struct res_task *task );
bool model_entity_free ( struct res_comp *comp );


#endif // MODEL_ENTITY_H_INCLUDED
