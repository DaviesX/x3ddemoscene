#ifndef PATHTRACERIMPL_H_INCLUDED
#define PATHTRACERIMPL_H_INCLUDED

#include <x3d/utility.h>
#include <x3d/light.h>

/*
 * <pathtracer> decl
 *
 */
struct pathtracer {
        int                     sample_count;
        struct light**          lights;
        int                     n_lights;
        struct util_aos*        aos;
        struct util_access*     acc;
};
/*
 * <pathtracer> public
 */
void pathtracer_init(struct pathtracer* self);
void pathtracer_free(struct pathtracer* self);
void pathtracer_set_sample_count(struct pathtracer* self, int sample_count);
void pathtracer_set_lights(struct pathtracer* self, struct light* lights, int n_lights);
void pathtracer_set_geometries(struct pathtracer* self, struct util_aos* aos, struct util_access* acc);
void pathtracer_render(struct pathtracer* self, struct util_aos* aos, struct util_access* acc, struct util_image* target);


#endif // PATHTRACERIMPL_H_INCLUDED
