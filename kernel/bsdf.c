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
        zero_obj(self);
        self->f_sample = f_sample;
        self->f_li = f_li;
        self->f_free = f_free;
}

void bsdf_model_free(struct bsdf_model* self)
{
        self->f_free(self);
}

void bsdf_model_sample(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected)
{
        self->f_sample(self, geom, incident, reflected);
}

void bsdf_model_integrate(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                          struct float_color3* lo, struct float_color3* li)
{
        self->f_li(self, geom, incident, reflected, lo, li);
}

/*
 * <bsdf_lambert> public
 */
struct bsdf_lambert* bsdf_lambert_create(struct float_color3* r)
{
        struct bsdf_lambert* self = alloc_obj(self);
        bsdf_model_init(&self->_parent,
                        (f_BSDF_Sample) bsdf_lambert_sample,
                        (f_BSDF_Li)     bsdf_lambert_integrate,
                        (f_BSDF_Free)   bsdf_lambert_free);
        self->r = *r;
        return self;
}

void bsdf_lambert_sample(struct bsdf_lambert* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected)
{
        // construct a random vector over the hemisphere in standard basis
        struct spherical3d sp;
        sp.the = uniform0_1()*M_PI_2;
        sp.phi = uniform0_1()*2.0f*M_PI;
        sp.r = 1.0f;
        struct vector3d d;
        spherical_to_vector3d(&sp, &d);
        // construct a basis S at the intersection, where normal points upward
        struct vector3d u, v, n;
        n = geom->n;
        cross_vector3d(&n, &incident->v, &u);
        cross_vector3d(&u, &n, &v);
        // transform from standard basis to basis S
        struct vector3d r;
        vector3d_comps(r.v[i] = d.x*u.v[i] + d.y*v.v[i] + d.z*n.v[i]);
        ray3d_build_t3(reflected, &geom->p, &r, 0.0f, FLOAT_MAX);
}

void bsdf_lambert_integrate(struct bsdf_lambert* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                            struct float_color3* lo, struct float_color3* li)
{
        float cos = dot_vector3d(&geom->n, &reflected->v);
        if (cos > 0.0f) {
                color3_comps(li->c[i] += cos*self->r.c[i]*lo->c[i]);
        }
}

void bsdf_lambert_free(struct bsdf_lambert* self)
{
}


/*
 * <bsdf_mirror> public
 */
struct bsdf_mirror* bsdf_mirror_create(struct float_color3* r)
{
        struct bsdf_mirror* self = alloc_obj(self);
        bsdf_model_init(&self->_parent,
                        (f_BSDF_Sample) bsdf_mirror_sample,
                        (f_BSDF_Li)     bsdf_mirror_integrate,
                        (f_BSDF_Free)   bsdf_mirror_free);
        self->r = *r;
        return self;
}

void bsdf_mirror_sample(struct bsdf_mirror* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected)
{
        struct vector3d refl;
        reflect_vector3d_r(&geom->n, &incident->v, &refl);
        ray3d_build_t3(reflected, &geom->p, &refl, 0.0f, FLOAT_MAX);
}

void bsdf_mirror_integrate(struct bsdf_mirror* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                           struct float_color3* lo, struct float_color3* li)
{
        float cos = dot_vector3d(&geom->n, &reflected->v);
        if (cos > 0.0f) {
                color3_comps(li->c[i] += cos*self->r.c[i]*lo->c[i]);
        }
}

void bsdf_mirror_free(struct bsdf_mirror* self)
{
}
