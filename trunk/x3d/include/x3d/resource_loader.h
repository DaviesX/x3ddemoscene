#ifndef RESOURCE_LOADER_H_INCLUDED
#define RESOURCE_LOADER_H_INCLUDED


enum RES_TYPE {
	RES_TYPE_GEOMETRY,
	RES_TYPE_KINEMATIC_SHAPE,
	RES_TYPE_MATERIAL,
	RES_TYPE_SURFACE,
	RES_TYPE_SHADER
};

enum RES_STATE {
	RES_EMPTY,
	RES_LOADING,
	RES_LOADED,
	RES_LOAD_FAIL
};

typedef struct res_header* res_ptr_t;
typedef struct res_loader* res_loader_t;
typedef uint32_t res_handle_t;

/*
 * functions' declaration
 */
void init_res_loader ( void );
void free_res_loader ( void );
void create_resource ( enum RES_TYPE type, res_ptr_t res );
enum RES_STATE load_resource ( char *filename, bool is_sync, res_loader_t *loader );
void reload_resource ( res_ptr_t res );
void free_resource ( res_ptr_t res );
enum RES_STATE checkout_resource ( res_ptr_t *res );
enum RES_STATE checkout_res_from_loader ( res_loader_t loader, int i, res_ptr_t *res );
void free_loader_resource ( res_loader_t loader );

/*
 * RI Interface
 */
void add_resource ( void *res_ptr, enum RES_TYPE type, res_ptr_t res );
void add_to_res_loader ( void *res_ptr, enum RES_TYPE type, res_loader_t loader );


#endif // RESOURCE_LOADER_H_INCLUDED
