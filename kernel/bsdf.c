#include <math/math.h>
#include <system/allocator.h>
#include <x3d/bsdf.h>


/*
 * <hitgeom> public
 */
void hitgeom_init(struct hitgeom* self, struct point3d* p, struct vector3d* n, struct vector2d* uv)
{
        self->p = *p;
        self->n = *n;
        if (uv) {
                self->uv = *uv;
        } else {
                init_vector2d(&self->uv);
        }
}

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

void bsdf_model_set_hitgeom(struct bsdf_model* self, struct hitgeom* geom)
{
        self->geom = geom;
}

void bsdf_model_set_incident(struct bsdf_model* self, struct ray3d* incident)
{
        self->incident = incident;
}

void bsdf_model_sample(struct bsdf_model* self, struct ray3d* reflected)
{
        self->f_sample(self, reflected);
}

void bsdf_model_integrate(struct bsdf_model* self, struct ray3d* reflected,
                          struct float_color3* lo, struct float_color3* li)
{
        self->f_li(self, reflected, lo, li);
}

/*
 * <bsdf_lambert> public
 */
struct bsdf_lambert* bsdf_lambert_create()
{
        struct bsdf_lambert* self = alloc_obj(self);
        bsdf_model_init(&self->_parent,
                        (f_BSDF_Sample) bsdf_lambert_sample,
                        (f_BSDF_Li)     bsdf_lambert_integrate,
                        (f_BSDF_Free)   bsdf_lambert_free);
        return self;
}

void bsdf_lambert_sample(struct bsdf_lambert* self, struct ray3d* reflected)
{
}

void bsdf_lambert_integrate(struct bsdf_lambert* self, struct ray3d* reflected,
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
        bsdf_model_init(&self->_parent,
                        (f_BSDF_Sample) bsdf_mirror_sample,
                        (f_BSDF_Li)     bsdf_mirror_integrate,
                        (f_BSDF_Free)   bsdf_mirror_free);
        return self;
}

void bsdf_mirror_sample(struct bsdf_mirror* self, struct ray3d* reflected)
{
}

void bsdf_mirror_integrate(struct bsdf_mirror* self, struct ray3d* reflected,
                           struct float_color3* lo, struct float_color3* li)
{
}

void bsdf_mirror_free(struct bsdf_mirror* self)
{
}
