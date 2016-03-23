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

enum SampleType {
        DiffuseSample,
        GlossySample,
        MixSample
};

typedef void (*f_BSDF_Sample) (struct bsdf_model* self, struct hitgeom* geom,
                               struct ray3d* incident, struct ray3d* reflected);
typedef void (*f_BSDF_Li) (struct bsdf_model* self, struct hitgeom* geom,
                           struct ray3d* incident, struct ray3d* reflected,
                           struct float_color3* lo, struct float_color3* li);

/*
 * <bsdf_model> decl
 * compute bsdf value and samples based on the reflection method
 * specified by the material.
 */
struct bsdf_model {
        enum SampleType sampt;
        int             num_samp;
};
/*
 * <bsdf_model> public
 */
void bsdf_model_set_sample_count(struct bsdf_model* self, int samp_count);
int bsdf_model_get_sample_count(struct bsdf_model* self);
void bsdf_model_sample(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected);
void bsdf_model_integrate(struct bsdf_model* self, struct hitgeom* geom, struct ray3d* incident, struct ray3d* reflected,
                          struct float_color3* lo, struct float_color3* li);


#endif // BSDF_H_INCLUDED
