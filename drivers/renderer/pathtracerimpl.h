#ifndef PATHTRACERIMPL_H_INCLUDED
#define PATHTRACERIMPL_H_INCLUDED

#include <x3d/geomcache.h>
#include <x3d/spatialaccess.h>
#include <x3d/bsdf.h>
#include <x3d/light.h>
#include <x3d/utility.h>

/*
 * <pathtracer> decl
 *
 */
struct pathtracer {
        int                     sample_count;
        struct light**          lights;
        int                     n_lights;
        struct geomcache*       aos;
        struct bsdf_model**     bsdfs;
        int                     n_bsdfs;
        struct spatial_access*  acc;
};
/*
 * <pathtracer> public
 */
void pathtracer_init(struct pathtracer* self);
void pathtracer_free(struct pathtracer* self);
void pathtracer_set_sample_count(struct pathtracer* self, int sample_count);
void pathtracer_set_lights(struct pathtracer* self, struct light** lights, int n_lights);
void pathtracer_set_geometries(struct pathtracer* self, struct geomcache* aos, struct spatial_access* acc);
void pathtracer_set_bsdfs(struct pathtracer* self, struct bsdf_model** bsdfs, int n_bsdfs);
void pathtracer_render(struct pathtracer* self, struct geomcache* aos, struct spatial_access* acc, struct util_image* target);


#endif // PATHTRACERIMPL_H_INCLUDED
