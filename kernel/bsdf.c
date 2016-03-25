#include <math/math.h>
#include <system/allocator.h>
#include <x3d/bsdf.h>


/*
 * <bsdf_model> public
 */
void bsdf_model_init(struct bsdf_model* self, f_BSDF_Sample f_sample, f_BSDF_Li f_li, f_BSDF_Free f_free)
{
}

void bsdf_model_free(struct bsdf_model* self)
{
}

void bsdf_model_set_sample_count(struct bsdf_model* self, int samp_count)
{
        self->num_samp = samp_count;
}

int bsdf_model_get_sample_count(struct bsdf_model* self)
{
        return self->num_samp;
}

void bsdf_model_sample(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected)
{
}

void bsdf_model_integrate(struct bsdf_model* self,  struct hitgeom* geom,
                          struct ray3d* incident, struct ray3d* reflected,
                          struct float_color3* lo, struct float_color3* li)
{
}

/*
 * <bsdf_lambert> public
 */
struct bsdf_lambert* bsdf_lambert_create()
{
        struct bsdf_lambert* self = alloc_obj(self);
        return self;
}

void bsdf_lambert_sample(struct bsdf_lambert* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected)
{
}

void bsdf_lambert_integrate(struct bsdf_lambert* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                            struct float_color3* lo, struct float_color3* li)
{
}

void bsdf_lambert_free(struct bsdf_lambert* self)
{
}


/*
 * <bsdf_mirror> public
 */
struct bsdf_mirror* bsdf_mirror_create()
{
        struct bsdf_mirror* self = alloc_obj(self);
        return self;
}

void bsdf_mirror_sample(struct bsdf_mirror* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected)
{
}

void bsdf_mirror_integrate(struct bsdf_mirror* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                           struct float_color3* lo, struct float_color3* li)
{
}

void bsdf_mirror_free(struct bsdf_mirror* self)
{
}
