#ifndef RAYTRACER_H_INCLUDED
#define RAYTRACER_H_INCLUDED

#include <x3d/utility.h>
#include <x3d/bsdf.h>
#include <math/math.h>


/*
 * <tintersect> decl
 * result of triangle intersection returned by the raytracer
 */
struct tintersect {
        int*            face;
        struct point3d  barycent;
        float           bias;
        struct hitgeom  geom;
};

/*
 * <raytracer> decl
 * triangular based raytracer module.
 */
struct raytracer {
        struct util_aos         aos;
        struct util_access      acc;
};
/*
 * <raytracer> public
 */
void raytracer_init(struct raytracer* self, struct util_aos* aos, struct util_access* acc);
void raytracer_free(struct raytracer* self);
bool raytracer_tintersect(struct raytracer* self, struct ray3d* ray, struct tintersect* t);
bool raytracer_occlusion(struct raytracer* self, struct ray3d* ray);


#endif // RAYTRACER_H_INCLUDED
