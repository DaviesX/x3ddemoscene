#ifndef PATHTRACERIMPL_H_INCLUDED
#define PATHTRACERIMPL_H_INCLUDED

#include <x3d/utility.h>

/*
 * <pathtracer> decl
 *
 */
struct pathtracer {
        int     sample_count;
};
/*
 * <pathtracer> public
 */
void pathtracer_init(struct pathtracer* self);
void pathtracer_free(struct pathtracer* self);
void pathtracer_set_sample_count(struct pathtracer* self, int sample_count);
void pathtracer_render(struct pathtracer* self, struct util_aos* aos, struct util_access* acc, struct util_image* target);


#endif // PATHTRACERIMPL_H_INCLUDED
