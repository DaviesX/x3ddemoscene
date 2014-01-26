/* geometry.c: geometric(shapes and geometry) handling all go here */
#include <renderer/geometry.h>



/*
 * shapes
 */
void init_shape ( void *shape_data, enum SHAPE_IDR idr, struct shape *shape )
{
}

void free_shape ( struct shape *shape )
{
}

struct stdshape *shape_to_stdshape ( struct shape *shape )
{
	return nullptr;
}

void shape_get_primitives ( struct shape *shape, struct alg_list *prim )
{
}

void shape_get_primitives_frustum ( struct shape *shape, struct frustum3d *f,
				    struct alg_list *prim )
{
}

void shape_get_primitives_box ( struct shape *shape, struct box3d *b,
				struct alg_list *prim )
{
}

void primitive_get_bound ( struct primitive *prim, struct box3d *bound )
{
}

/*
 * geometry
 */
void create_geometry ( res_handle_t shape, res_handle_t mater,
		       res_handle_t shader, struct geometry *geo )
{
}

void free_geometry ( struct geometry *geo )
{
}

void geometry_get_shape ( struct geometry *geo, struct shape *shape )
{
}

void geometry_set_transform ( struct matrix4x4 *trans, struct geometry *geo )
{
}
