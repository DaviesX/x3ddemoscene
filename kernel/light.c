#include <system/allocator.h>
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


/*
 * <light_point> public
 */
struct light_point* light_point_create(struct float_color3* flux, struct point3d* p, float radius)
{
        struct light_point* self = alloc_obj(self);
        return self;
}

void light_point_free(struct light_point* self)
{
}

void light_point_sample_at(struct light_point* self, struct point3d* p, struct vector3d* n, struct float_color3* i)
{
}

void light_point_sample_at2(struct light_point* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i)
{
}

/*
 * <light_rectangular> public
 */
struct light_rectangular* light_rect_create(struct float_color3* flux,
                struct point3d* p0, struct point3d* p1, struct point3d* p2, struct point3d* p3)
{
        struct light_rectangular* self = alloc_obj(self);
        return self;
}

void light_rect_free(struct light_rectangular* self)
{
}

void light_rect_sample_at(struct light_rectangular* self, struct point3d* p, struct vector3d* n, struct float_color3* i)
{
}

void light_rect_sample_at2(struct light_rectangular* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i)
{
}
