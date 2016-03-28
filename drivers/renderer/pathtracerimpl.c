#include <math/math.h>
#include <x3d/raytracer.h>
#include <x3d/bsdf.h>
#include <x3d/projectionprobe.h>
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

void pathtracer_set_projection_probe(struct pathtracer* self, struct projection_probe* probe)
{
        self->probe = probe;
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

        // compute direct lighting
        struct float_color3 le = {0.0f, 0.0f, 0.0f};
        int i;
        for (i = 0; i < data->n_lights; i ++) {
                struct ray3d illumray;
                struct float_color3 inten;
                light_sample_at2(data->lights[i], &inters.geom.p, &illumray, &inten);
                if (is_color_black(&inten) || raytracer_occlusion(&data->rt, &illumray)) {
                        // 0 intensity or shadowed
                        continue;
                }
                bsdf_model_integrate(bsdf, &inters.geom, ray, &illumray, &inten, &le);
        }

        int n;
        for (n = 0; n < c_NumBranchedSample; n ++) {
                struct ray3d reflected;
                bsdf_model_sample(bsdf, &inters.geom, ray, &reflected);
                struct float_color3 lo;
                __pathtracer_trace_at(data, &reflected, &lo);
                bsdf_model_integrate(bsdf, &inters.geom, ray, &reflected, &lo, li);
        }
        color3_comps(li->c[i] = li->c[i]/c_NumBranchedSample + le.c[i]/data->n_lights);
}

static void __pathtracer_integrate_at(struct pathtracer* self, int i, int j, int n, struct float_color3* fn)
{
        // simply trace a single ray
        struct ray3d initial;
        projprobe_sample_at(self->probe, i, j, &initial);
//        struct point3d p0 = {278.0f*c_Proportion, 273.0f*c_Proportion, -800.0f*c_Proportion};
//        struct vector3d v = {x*0.025f, y*0.025f, 0.035f*2.0f};
//        normalize_vector3d_u(&v);
//        ray3d_build_t3(&initial, &p0, &v, 1.0f, FLOAT_MAX);

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
                        struct float_color3 expected;
                        init_color3(0.0f, &expected);
                        int n;
                        for (n = 0; n < self->sample_count; n ++) {
                                __pathtracer_integrate_at(self, i, j, self->sample_count, &expected);
                        }

                        struct float_color3* px = u_image_read(target, 0, i, j);
                        *px = expected;
                }
        }
}

/*
 * <pathtracer> test cases
 */
// hdr functions
static uint32_t __gamma_rgb_radiance(struct float_color3* x)
{
        float post_rad[3];
        color3_comps(post_rad[i] = clamp(powf(x->c[i], 1.0f/2.2f), 0.0f, 1.0f));

        return ((uint8_t) (0XFF) << 24) | \
               ((uint8_t)(post_rad[0]*255.0f) << 16) | \
               ((uint8_t)(post_rad[1]*255.0f) << 8) | \
               ((uint8_t)(post_rad[2]*255.0f) << 0);
}

static double __log_illuminance(struct float_color3* x)
{
        double illu = 0.3f*x->rgb[0] + 0.6f*x->rgb[1] + 0.1f*x->rgb[2];
        return log(illu + 1.25);
}

/* from: http://filmicgames.com/archives/75, Filmic HDR */
static const float A = 0.15f;
static const float B = 0.50f;
static const float C = 0.10f;
static const float D = 0.20f;
static const float E = 0.02f;
static const float F = 0.30f;
static const struct float_color3 W = {11.2f, 11.2f, 11.2f};

static void __inv_filimc_curve(struct float_color3* x, struct float_color3* t)
{
        color3_comps(t->c[i] = 1.0f/(((x->c[i]*(A*x->c[i] + C*B) + D*E)/(x->c[i]*(A*x->c[i] + 1*B) + D*F)) - E/F));
}

static void __filmic_curve(struct float_color3* x, struct float_color3* t)
{
        color3_comps(t->c[i] = ((x->c[i]*(A*x->c[i] + C*B) + D*E)/(x->c[i]*(A*x->c[i] + 1*B) + D*F)) - E/F);
}

static void __hdr_rgb_radiance(struct float_color3* x, float avg_illum, struct float_color3* reexp)
{
        const float exp_bias = 5.0f;
        struct float_color3 r;
        color3_comps(r.c[i] = x->c[i]*exp_bias*avg_illum);

        struct float_color3 c;
        struct float_color3 white_scale;
        __filmic_curve(&r, &c);
        __inv_filimc_curve((struct float_color3*) &W, &white_scale);

        mul_color3(&c, &white_scale, &r);
        *reexp = r;
}

static double __compute_geometric_average_illuminance(struct util_image* radiances)
{
        int j, i, w, h;
        u_image_get_level0_dimension(radiances, &w, &h);
        double log_sum = 0.0;
        for (j = 0; j < h; j ++) {
                for (i = 0; i < w; i ++) {
                        struct float_color3* px = u_image_read(radiances, 0, i, j);
                        double log_illum = __log_illuminance(px);
                        log_sum += log_illum;
                }
        }
        return exp(1.0f/(w*h)*log_sum);
}

static void __evaluate_hdr_target(struct util_image* radiances, struct util_image* hdr)
{
        double exposure = __compute_geometric_average_illuminance(radiances);

        int j, i, w, h;
        u_image_get_level0_dimension(radiances, &w, &h);
        for (j = 0; j < h; j ++) {
                for (i = 0; i < w; i ++) {
                        struct float_color3* px = u_image_read(radiances, 0, i, j);
                        uint32_t *hdrpx = u_image_read(hdr, 0, i, j);

                        struct float_color3 rgb;
                        __hdr_rgb_radiance(px, exposure, &rgb);
                        *hdrpx = __gamma_rgb_radiance(&rgb);
                }
        }
}

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
        maters[WHITE]  = &bsdf_lambert_create("white", &r_white)->_parent;
        maters[RED]    = &bsdf_lambert_create("red", &r_red)->_parent;
        maters[GREEN]  = &bsdf_lambert_create("green", &r_green)->_parent;
        maters[MIRROR] = &bsdf_mirror_create("perfect", &r_perfect)->_parent;

        const float c_PowerScale = 15.0f;
        struct light* lights[10];
        struct point3d p = {275.0f*c_Proportion, 350.0f*c_Proportion, 249.5f*c_Proportion};
        struct float_color3 flux = {245.0f*c_PowerScale, 191.0f*c_PowerScale, 129.0f*c_PowerScale};
        lights[0] = &light_point_create("test_light0", &flux, &p, 10.0f*c_Proportion)->_parent;

        const int w = 800, h = 600;
        struct perspective_probe* probe = persprobe_create(PPMImageOutput, nullptr);
        struct vector3d up = {0.0f, 1.0f, 0.0f};
        struct point3d p0 = {278*c_Proportion, 273*c_Proportion, -800*c_Proportion};
        struct vector3d dir = {0.0, 0.0, 1.0f};
        projprobe_set_position(&probe->_parent, &p0);
        projprobe_set_output_format(&probe->_parent, w, h, Color32Mode);
        persprobe_set_direction(probe, &up, &dir);
        persprobe_set_range(probe, 1.0f, 1.0f, FLOAT_MAX);
        persprobe_set_optics(probe, 35, 0.035897436f, 0.0f);

        struct pathtracer pt;
        pathtracer_init(&pt);
        pathtracer_set_geometries(&pt, gc, &acc._parent);
        pathtracer_set_bsdfs(&pt, maters, 5);
        pathtracer_set_lights(&pt, lights, 1);
        pathtracer_set_projection_probe(&pt, &probe->_parent);

        // radiance target
        struct util_image target;
        u_image_init(&target, 1, UtilImgRGBRadiance, w, h);
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

        // hdr target
        struct util_image hdr;
        u_image_init(&hdr, 1, UtilImgRGBA32, w, h);
        u_image_alloc(&hdr, 0);
        __evaluate_hdr_target(&target, &hdr);

        // output image
        u_image_export_to_file(&hdr, fopen("pathtracer-test-result.ppm", "w"));
        u_image_free(&target);

        geomcache_free(gc);
        free_fix(gc);

        u_access_free(&acc._parent);
}
