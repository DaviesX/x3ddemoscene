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
        zero_obj(self);
        pathtracer_set_sample_count(self, 1);
}

void pathtracer_free(struct pathtracer* self)
{
}

void pathtracer_set_sample_count(struct pathtracer* self, int sample_count)
{
        self->sample_count = sample_count;
}

void pathtracer_set_lights(struct pathtracer* self, struct light* lights, int n_light)
{
}

void pathtracer_set_geometries(struct pathtracer* self, struct util_aos* aos, struct util_access* acc)
{
}

struct tracer_data {
        struct util_aos*        aos;
        struct light**          lights;
        int                     n_lights;
        float                   plight;
        struct raytracer        rt;
        int                     depth;
};

static void __pathtracer_data_init(struct tracer_data* self, struct util_aos* aos,
                                   struct light** lights, int n_lights, struct raytracer* rt)
{
        self->aos = aos;
        self->lights = lights;
        self->n_lights = n_lights;
        self->plight = (float) 1.0f/n_lights;
        self->rt = *rt;
        self->depth = 0;
}

static void __pathtracer_trace_at(struct tracer_data* data, struct ray3d* ray, struct float_color3* li)
{
        init_color3(0.0f, li);

        struct tintersect inters;
        if (data->depth ++ > 4 || !raytracer_tintersect(&data->rt, ray, &inters)) {
                return;
        }
        const int c_NumBranchedSample = 4;

        // @fixme needed to extract from material
        struct bsdf_model bsdf;
        bsdf_model_set_hitgeom(&bsdf, &inters.geom);
        bsdf_model_set_incident(&bsdf, ray);

        // compute direct lighting
        bsdf_model_set_sample_count(&bsdf, data->n_lights);
        int i;
        for (i = 0; i < data->n_lights; i ++) {
                struct ray3d illumray;
                struct float_color3 inten;
                light_sample_at2(data->lights[i], &inters.geom.p, &illumray, &inten);
                if (is_color_black(&inten) || raytracer_occlusion(&data->rt, &illumray)) {
                        // 0 intensity or shadowed
                        continue;
                }
                bsdf_model_integrate(&bsdf, &illumray, &inten, li);
        }

        bsdf_model_set_sample_count(&bsdf, c_NumBranchedSample);
        int n;
        for (n = 0; n < c_NumBranchedSample; n ++) {
                struct ray3d reflected;
                bsdf_model_sample(&bsdf, &reflected);
                struct float_color3 lo;
                __pathtracer_trace_at(data, &reflected, &lo);
                bsdf_model_integrate(&bsdf, &reflected, &lo, li);
        }
}

static void __pathtracer_compute_at(struct pathtracer* self, float x, float y, int n, struct float_color3* fn)
{
        // simply trace a single ray
        struct ray3d initial;
        struct point3d p0 = {0.0f, 0.0f, 0.0f};
        struct point3d p1 = {x, y, 1.0f};
        build_line3d_t(&p0, &p1, 1.0f, FLOAT_MAX, &initial);

        struct raytracer rt;
        raytracer_init(&rt, self->aos, self->acc);

        struct tracer_data data;
        __pathtracer_data_init(&data, self->aos, self->lights, self->n_lights, &rt);
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
                                __pathtracer_compute_at(self, x, y, n, &fn);
                                add_color3(&expected, &fn, &expected);
                        }
                        float p = (float) 1.0f/self->sample_count;
                        scale_color3(p, &expected, &expected);

                        struct float_color3* px = u_image_read(target, 0, i, j);
                        *px = expected;
                }
        }
}
