#include <x3d/colorspectrum.h>
#include <x3d/light.h>


/*
 * <light> public
 */
void light_init(struct light* self, f_Light_Sample_At f_sample, f_Light_Sample_At2 f_sample2, f_Light_Free f_free)
{
        self->f_sample = f_sample;
        self->f_sample2 = f_sample2;
        self->f_free = f_free;
}

void light_free(struct light* self)
{
        self->f_free(self);
}

void light_sample_at(struct light* self, struct point3d* p, struct vector3d* n, struct float_color3* i)
{
        self->f_sample(self, p, n, i);
}

void light_sample_at2(struct light* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i)
{
        self->f_sample2(self, p0, illumray, i);
}
