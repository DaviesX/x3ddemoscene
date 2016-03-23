#include <math/math.h>
#include <x3d/bsdf.h>


/*
 * <bsdf_model> public
 */
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
