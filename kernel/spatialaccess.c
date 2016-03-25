#include <system/allocator.h>
#include <x3d/spatialaccess.h>


/*
 * <util_access> public
 */
void u_access_init(struct spatial_access* self, enum UtilAccessorType type,
                              f_Util_Access_Build f_build, f_Util_Access_Free f_free)
{
        self->type = type;
        self->f_build = f_build;
        self->f_free = f_free;
}

void u_access_set_objects(struct spatial_access* self, struct box3d* simplex, int n_objects)
{
        self->simplex = simplex;
        self->n_objects = n_objects;
}

struct box3d* u_access_get_objects(struct spatial_access* self, int* n_objects)
{
        *n_objects = self->n_objects;
        return self->simplex;
}

void u_access_free(struct spatial_access* self)
{
	if (self == nullptr) {
		return ;
	}
	self->f_free(self);
}

void u_access_build(struct spatial_access* self)
{
        self->f_build(self);
}

/*
 * <util_linear> public
 */
void u_linear_init(struct spatial_linear* self, struct box3d* simplex, int n_objects)
{
        u_access_init(&self->_parent, UtilAccessorLinear,
                      (f_Util_Access_Build) u_linear_build,
                      (f_Util_Access_Free) u_linear_free);
        u_access_set_objects(&self->_parent, simplex, n_objects);
}

void u_linear_free(struct spatial_linear* self)
{
        zero_obj(self);
}

void u_linear_build(struct spatial_linear* linear)
{
        return ;        // nothing is needed
}
