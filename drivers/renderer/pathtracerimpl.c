#include <math/math.h>
#include <x3d/raytracer.h>
#include <x3d/bsdf.h>
#include <x3d/colorspectrum.h>
#include <x3d/debug.h>
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

void pathtracer_set_lights(struct pathtracer* self, struct light** lights, int n_lights)
{
        self->lights = lights;
        self->n_lights = n_lights;
}

void pathtracer_set_geometries(struct pathtracer* self, struct geomcache* aos, struct spatial_access* acc)
{
        self->aos = aos;
        self->acc = acc;
}

void pathtracer_set_bsdfs(struct pathtracer* self, struct bsdf_model** bsdfs, int n_bsdfs)
{
        self->bsdfs = bsdfs;
        self->n_bsdfs = n_bsdfs;
}

struct tracer_data {
        struct geomcache*        aos;
        struct light**          lights;
        int                     n_lights;
        struct bsdf_model**     bsdfs;
        int                     n_bsdfs;
        struct raytracer        rt;
        int                     depth;
};

static void __pathtracer_data_init(struct tracer_data* self, struct geomcache* aos,
                                   struct light** lights, int n_lights,
                                   struct bsdf_model** bsdfs, int n_bsdfs, struct raytracer* rt)
{
        self->aos = aos;
        self->lights = lights;
        self->n_lights = n_lights;
        self->bsdfs = bsdfs;
        self->n_bsdfs = n_bsdfs;
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
        const int c_NumBranchedSample = 1;

        int mater_id = 0;
        if (geomcache_has_material_id(data->aos)) {
                mater_id = *geomcache_material_id_at(data->aos, inters.face[0]);
        }
        struct bsdf_model* bsdf = data->bsdfs[mater_id];
        bsdf_model_set_hitgeom(bsdf, &inters.geom);
        bsdf_model_set_incident(bsdf, ray);

        // compute direct lighting
        bsdf_model_set_sample_count(bsdf, data->n_lights);
        int i;
        for (i = 0; i < data->n_lights; i ++) {
                struct ray3d illumray;
                struct float_color3 inten;
                light_sample_at2(data->lights[i], &inters.geom.p, &illumray, &inten);
                if (is_color_black(&inten) || raytracer_occlusion(&data->rt, &illumray)) {
                        // 0 intensity or shadowed
                        continue;
                }
                bsdf_model_integrate(bsdf, &illumray, &inten, li);
        }

        bsdf_model_set_sample_count(bsdf, c_NumBranchedSample);
        int n;
        for (n = 0; n < c_NumBranchedSample; n ++) {
                struct ray3d reflected;
                bsdf_model_sample(bsdf, &reflected);
                struct float_color3 lo;
                __pathtracer_trace_at(data, &reflected, &lo);
                bsdf_model_integrate(bsdf, &reflected, &lo, li);
        }
}

static void __pathtracer_integrate_at(struct pathtracer* self, float x, float y, int n, struct float_color3* fn)
{
        // simply trace a single ray
        struct ray3d initial;
        struct point3d p0 = {278.0f*c_Proportion, 273.0f*c_Proportion, -800.0f*c_Proportion};
        struct vector3d v = {x*0.025f, y*0.025f, 0.035f*2.0f};
        normalize_vector3d_u(&v);
        ray3d_build_t3(&initial, &p0, &v, 1.0f, FLOAT_MAX);

        struct raytracer rt;
        raytracer_init(&rt, self->aos, self->acc);

        struct tracer_data data;
        __pathtracer_data_init(&data, self->aos,
                               self->lights, self->n_lights,
                               self->bsdfs, self->n_bsdfs, &rt);
        struct float_color3 samp;
        __pathtracer_trace_at(&data, &initial, &samp);
        float p = 1.0f/n;
        color3_comps(fn->c[i] += p*samp.c[i]);

        raytracer_free(&rt);
}

void pathtracer_render(struct pathtracer* self, struct util_image* target)
{
        int w, h;
        u_image_get_level0_dimension(target, &w, &h);
        if (w < 2 || h < 2) return;
        int i, j;
        for (j = 0; j < h; j ++) {
                for (i = 0; i < w; i ++) {
                        float x =   2.0f*(float) i/(w - 1) - 1.0f;
                        float y = -(2.0f*(float) j/(h - 1) - 1.0f);

                        struct float_color3 expected;
                        init_color3(0.0f, &expected);
                        int n;
                        for (n = 0; n < self->sample_count; n ++) {
                                __pathtracer_integrate_at(self, x, y, self->sample_count, &expected);
                        }

                        struct float_color3* px = u_image_read(target, 0, i, j);
                        *px = expected;
                }
        }
}

/*
 * <pathtracer> test cases
 */
void pathtracer_test_init(struct alg_var_set* envir) {};
void pathtracer_test_free(struct alg_var_set* envir) {};
enum DebugPosition* pathtracer_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
void pathtracer_test(struct alg_var_set* envir)
{
        struct geomcache* gc = geomcache_build_test_sample();
        struct spatial_linear acc;
        int n_objects;
        struct box3d* simplexes = geomcache_export_simplexes(gc, &n_objects);
        u_linear_init(&acc, simplexes, n_objects);
        u_linear_build(&acc);

        struct bsdf_model* maters[10];
        struct float_color3 r_white = {1.0f, 0.94f, 0.701176471f};
        struct float_color3 r_red = {1.0f, 0.0f, 0.0f};
        struct float_color3 r_green = {0.0f, 1.0f, 0.0f};
        struct float_color3 r_perfect = {0.8f, 0.8f, 0.8f};
        maters[WHITE]  = &bsdf_lambert_create(&r_white)->_parent;
        maters[RED]    = &bsdf_lambert_create(&r_red)->_parent;
        maters[GREEN]  = &bsdf_lambert_create(&r_green)->_parent;
        maters[MIRROR] = &bsdf_mirror_create(&r_perfect)->_parent;

        const float c_PowerScale = 20.0f;
        struct light* lights[10];
        struct point3d p = {275.0f*c_Proportion, 300.0f*c_Proportion, 249.5f*c_Proportion};
        struct float_color3 flux = {245.0f*c_PowerScale, 191.0f*c_PowerScale, 129.0f*c_PowerScale};
        lights[0] = &light_point_create(&flux, &p, 10.0f*c_Proportion)->_parent;

        struct pathtracer pt;
        pathtracer_init(&pt);
        pathtracer_set_geometries(&pt, gc, &acc._parent);
        pathtracer_set_bsdfs(&pt, maters, 5);
        pathtracer_set_lights(&pt, lights, 1);

        struct util_image target;
        u_image_init(&target, 1, UtilImgRGBRadiance, 800, 600);
        u_image_alloc(&target, 0);
        pathtracer_set_sample_count(&pt, 64);
        pathtracer_render(&pt, &target);

        bsdf_model_free(maters[WHITE]);
        bsdf_model_free(maters[RED]);
        bsdf_model_free(maters[GREEN]);
        bsdf_model_free(maters[MIRROR]);
        free_fix(maters[WHITE]);
        free_fix(maters[RED]);
        free_fix(maters[GREEN]);
        free_fix(maters[MIRROR]);

        light_free(lights[0]);
        free_fix(lights[0]);

        u_image_export_to_file(&target, fopen("pathtracer-test-result.ppm", "w"));
        u_image_free(&target);

        geomcache_free(gc);
        free_fix(gc);

        u_access_free(&acc._parent);
}
