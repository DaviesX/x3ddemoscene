#ifndef RESLOADER_H_INCLUDED
#define RESLOADER_H_INCLUDED


#include <staging.h>

enum RES_IDR {
        RES_RENDERABLE,
        RES_MATERIAL,
        RES_SHADER,
        RES_TEXTURE,
        RES_FONT,
        RES_MODEL_ENTITY,
        RES_KINEMATIC_SHAPE,
        RES_LOGICAL,
        RES_BONDER,
        RES_DLPROGRAM,
        RES_BINARY_BLOB
};

enum RES_STATE {
        RES_STATE_EMPTY,
        RES_STATE_LOADING,
        RES_STATE_LOADED,
        RES_STATE_FAILED
};

struct res_loader;
struct res_task;
struct res_comp;
typedef void* res_ptr_t;


/*
 * functions' declaration
 */
struct res_loader *create_res_loader ( void );
void free_res_loader ( struct res_loader *loader );
void res_loader_flush ( struct res_loader *loader );
void res_loader_sanitize ( struct res_loader *loader );
struct res_task *res_loader_load ( char *filename, enum RES_IDR type, bool is_async,
                                   struct res_loader *loader );
struct res_task *res_loader_save ( char *filename, res_ptr_t res_ptr, enum RES_IDR type,
                                   bool is_async, struct res_loader *loader );
struct res_task *res_loader_pop ( enum RES_IDR type, struct res_loader *loader );

enum RES_STATE res_loader_task_query ( struct res_task *task );
res_ptr_t res_loader_task_get ( struct res_task *task, int *n_inst, enum RES_STATE *state );
void res_loader_task_free ( struct res_task *task );

/*
 * resource components
 */
struct res_comp *res_comp_alloc_unit ( struct res_task *task );
void *res_comp_alloc_vertex ( int size, int count, struct res_comp *comp );
void *res_comp_alloc_normal ( int size, int count, struct res_comp *comp );
void *res_comp_alloc_uv ( int size, int count, struct res_comp *comp );
void *res_comp_alloc_tangent ( int size, int count, struct res_comp *comp );
void *res_comp_alloc_face ( int num_vertex, int count, struct res_comp *comp );

void *res_comp_alloc_chars ( char *name, int count, struct res_comp *comp );
void *res_comp_alloc_ints ( char *name, int count, struct res_comp *comp );
void *res_comp_alloc_floats ( char *name, int count, struct res_comp *comp );
void *res_comp_alloc_refptrs ( char *name, int count, struct res_comp *comp );

void *res_comp_alloc_image ( char *name, int w, int h, int size, struct res_comp *comp );

void *res_comp_shrink ( void *ptr, int real_count );


#endif // RESLOADER_H_INCLUDED
