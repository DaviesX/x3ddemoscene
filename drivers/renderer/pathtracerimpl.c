#include <math/math.h>
#include <x3d/raytracer.h>
#include <x3d/bsdf.h>
#include <x3d/colorspectrum.h>
#include "pathtracerimpl.h"


/*
 * <pathtracer> public
 */
void pathtracer_init(struct pathtracer* self)
{
}

void pathtracer_free(struct pathtracer* self)
{
}

void pathtracer_set_sample_count(struct pathtracer* self, int sample_count)
{
        self->sample_count = sample_count;
}

struct tracer_data {
        struct util_aos*        aos;
        struct raytracer        rt;
        int                     depth;
};

static void __pathtracer_trace_at(struct tracer_data* data, struct ray3d* ray, struct float_color3* li)
{
        struct tintersect inters;
        if (data->depth >= 5 || !raytracer_tintersect(&data->rt, ray, &inters)) {
                init_color3(0.0f, li);
                return;
        }
        const int c_NumBranchedSample = 4;

        // @fixme needed to extract from material
        struct bsdf_model bsdf;
        bsdf_model_set_sample_count(&bsdf, c_NumBranchedSample);

        init_color3(0.0, li);
        int n;
        for (n = 0; n < c_NumBranchedSample; n ++) {
                struct ray3d reflected;
                bsdf_model_sample(&bsdf, &inters.geom, ray, &reflected);
                struct float_color3 lo;
                __pathtracer_trace_at(data, &reflected, &lo);
                bsdf_model_integrate(&bsdf, &inters.geom, ray, &reflected, &lo, li);
        }
}

static void __pathtracer_compute_at(struct util_aos* aos, struct util_access* acc,
                                    float x, float y, int n, struct float_color3* fn)
{
        // simply trace a single ray
        struct ray3d initial;
        struct point3d p0 = {0.0f, 0.0f, 0.0f};
        struct point3d p1 = {x, y, 1.0f};
        build_line3d_t(&p0, &p1, 1.0f, FLOAT_MAX, &initial);

        struct raytracer rt;
        raytracer_init(&rt, aos, acc);

        struct tracer_data data;
        data.aos = aos;
        data.depth = 0;
        data.rt = rt;
        __pathtracer_trace_at(&data, &initial, fn);

        raytracer_free(&rt);
}

void pathtracer_render(struct pathtracer* self, struct util_aos* aos, struct util_access* acc, struct util_image* target)
{
        int w, h;
        u_image_get_level0_dimension(target, &w, &h);
        if (w < 2 || h < 2) return;
        int i, j;
        for (j = 0; j < h; j ++) {
                for (i = 0; i < w; i ++) {
                        float x = (float) i/(w - 1);
                        float y = (float) j/(h - 1);

                        struct float_color3 expected;
                        init_color3(0.0f, &expected);
                        int n;
                        for (n = 0; n < self->sample_count; n ++) {
                                struct float_color3 fn;
                                __pathtracer_compute_at(aos, acc, x, y, n, &fn);
                                add_color3(&expected, &fn, &expected);
                        }
                        float p = (float) 1.0f/self->sample_count;
                        scale_color3(p, &expected, &expected);

                        struct float_color3* px = u_image_read(target, 0, i, j);
                        *px = expected;
                }
        }
}
