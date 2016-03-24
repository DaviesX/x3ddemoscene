#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <math/math.h>
#include <x3d/colorspectrum.h>

struct light;

typedef void (*f_Light_Sample_At) (struct light* self, struct point3d* p, struct vector3d* n, struct float_color3* i);
typedef void (*f_Light_Sample_At2) (struct light* self, struct point3d* p0,
                                    struct ray3d* illumray, struct float_color3* i);
typedef void (*f_Light_Free) (struct light* self);

/*
 * <light> decl
 */
struct light {
        f_Light_Sample_At       f_sample;
        f_Light_Sample_At2      f_sample2;
        f_Light_Free            f_free;
};
/*
 * <light> public
 */
void light_init(struct light* self, f_Light_Sample_At f_sample,
                f_Light_Sample_At2 f_sample2,
                f_Light_Free f_free);
void light_free(struct light* self);
void light_sample_at(struct light* self, struct point3d* p, struct vector3d* n, struct float_color3* i);
void light_sample_at2(struct light* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i);


#endif // LIGHT_H_INCLUDED
