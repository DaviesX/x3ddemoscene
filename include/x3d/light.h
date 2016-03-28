#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <math/math.h>
#include <x3d/colorspectrum.h>

struct light;

typedef void (*f_Light_Sample_At) (struct light* self, struct point3d* p, struct vector3d* n, struct float_color3* i);
typedef float (*f_Light_Sample_At2) (struct light* self, struct point3d* p0,
                                     struct ray3d* illumray, struct float_color3* i);
typedef void (*f_Light_Free) (struct light* self);

/*
 * <light> decl
 */
struct light {
        f_Light_Sample_At       f_sample;
        f_Light_Sample_At2      f_sample2;
        f_Light_Free            f_free;
        char*                   name;
};
/*
 * <light> public
 */
void light_init(struct light* self,
                const char* name,
                f_Light_Sample_At f_sample,
                f_Light_Sample_At2 f_sample2,
                f_Light_Free f_free);
void light_free(struct light* self);
const char* light_get_name(struct light* self);
void light_sample_at(struct light* self, struct point3d* p, struct vector3d* n, struct float_color3* i);
float light_sample_at2(struct light* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i);

/*
 * <light_point> decl
 * define a point light source without having a concrete geometry.
 * It is possible that the light source can have zero or non-zero radius.
 */
struct light_point {
        struct light            _parent;
        struct float_color3     inten;
        struct float_color3     flux;
        struct point3d          center;
        float                   radius;
        float                   half_r2;
};
/*
 * <light_point> public
 */
struct light_point* light_point_create(const char* name, struct float_color3* flux, struct point3d* p, float radius);
void light_point_free(struct light_point* self);
void light_point_sample_at(struct light_point* self, struct point3d* p, struct vector3d* n, struct float_color3* i);
float light_point_sample_at2(struct light_point* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i);

/*
 * <light_rectangular> decl
 * define a single side rectangular light source without having a concrete geometry.
 * It is possible that the light source can have zero or non-zero area.
 */
struct light_rectangular {
        struct light            _parent;
        struct float_color3     inten;
        struct float_color3     flux;
        float                   area;
        struct vector3d         n;
        struct point3d          p0;
        struct point3d          p1;
        struct point3d          p2;
        struct point3d          p3;
};
/*
 * <light_rectangular> public
 */
struct light_rectangular* light_rect_create(const char* name, struct float_color3* flux,
                struct point3d* p0, struct point3d* p1, struct point3d* p2, struct point3d* p3);
void light_rect_free(struct light_rectangular* self);
void light_rect_sample_at(struct light_rectangular* self, struct point3d* p, struct vector3d* n, struct float_color3* i);
float light_rect_sample_at2(struct light_rectangular* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i);


#endif // LIGHT_H_INCLUDED
