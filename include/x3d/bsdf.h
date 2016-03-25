#ifndef BSDF_H_INCLUDED
#define BSDF_H_INCLUDED

#include <math/math.h>
#include <x3d/colorspectrum.h>


/*
 * <hitgeom> decl
 * contain information of the geometry at the point being hit;
 */
struct hitgeom {
        struct point3d  p;
        struct vector3d n;
        struct vector2d uv;
};

struct bsdf_model;
struct hitgeom;

typedef void (*f_BSDF_Sample) (struct bsdf_model* self, struct hitgeom* geom,
                               struct ray3d* incident, struct ray3d* reflected);
typedef void (*f_BSDF_Li) (struct bsdf_model* self, struct hitgeom* geom,
                           struct ray3d* incident, struct ray3d* reflected,
                           struct float_color3* lo, struct float_color3* li);
typedef void (*f_BSDF_Free) (struct bsdf_model* self);

/*
 * <bsdf_model> decl
 * compute bsdf value and samples based on the reflection method
 * specified by the material.
 */
struct bsdf_model {
        f_BSDF_Sample   f_sample;
        f_BSDF_Li       f_li;
        f_BSDF_Free     f_free;
        int             num_samp;
};
/*
 * <bsdf_model> public
 */
void    bsdf_model_init(struct bsdf_model* self, f_BSDF_Sample f_sample, f_BSDF_Li f_li, f_BSDF_Free f_free);
void    bsdf_model_free(struct bsdf_model* self);
void    bsdf_model_set_sample_count(struct bsdf_model* self, int samp_count);
int     bsdf_model_get_sample_count(struct bsdf_model* self);
void    bsdf_model_sample(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected);
void    bsdf_model_integrate(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                             struct float_color3* lo, struct float_color3* li);

/*
 * <bsdf_lambert> decl
 */
struct bsdf_lambert {
        struct bsdf_model       _parent;
};
/*
 * <bsdf_lambert> public
 */
struct bsdf_lambert* bsdf_lambert_create();
void    bsdf_lambert_sample(struct bsdf_lambert* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected);
void    bsdf_lambert_integrate(struct bsdf_lambert* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                               struct float_color3* lo, struct float_color3* li);
void    bsdf_lambert_free(struct bsdf_lambert* self);

/*
 * <bsdf_mirror> decl
 */
struct bsdf_mirror {
        struct bsdf_model       _parent;
};
/*
 * <bsdf_mirror> public
 */
struct bsdf_mirror* bsdf_mirror_create();
void    bsdf_mirror_sample(struct bsdf_mirror* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected);
void    bsdf_mirror_integrate(struct bsdf_mirror* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                              struct float_color3* lo, struct float_color3* li);
void    bsdf_mirror_free(struct bsdf_mirror* self);


#endif // BSDF_H_INCLUDED
