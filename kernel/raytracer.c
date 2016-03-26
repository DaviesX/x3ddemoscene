#include <math/math.h>
#include <system/log.h>
#include <x3d/debug.h>
#include <x3d/raytracer.h>


/*
 * <raytracer> public
 */
void raytracer_init(struct raytracer* self,
                    struct geomcache* aos,
                    struct spatial_access* acc)
{
        self->acc = *acc;
        self->aos = *aos;
}

void raytracer_free(struct raytracer* self)
{
        // do nothing
}

struct tintersect_data {
        struct tintersect*      inters;
        struct geomcache*        aos;
        struct ray3d*           ray;
        float                   t_hit;
        int                     i;
};

static bool __raytracer_simplex(int i, struct box3d* simplex, struct tintersect_data* data)
{
        return occlude_line_box3d(data->ray, simplex);
}

static bool __raytracer_triangle_tintersect(int i, struct tintersect_data* data)
{
        int* face = geomcache_index_at(data->aos, i*3);
        struct point3d* p0 = geomcache_vertex_at(data->aos, face[0]);
        struct point3d* p1 = geomcache_vertex_at(data->aos, face[1]);
        struct point3d* p2 = geomcache_vertex_at(data->aos, face[2]);
        float bias, t;
        struct point3d b;
        if (!intersect_ray_triangle3d(data->ray, p0, p1, p2, b.v, &t, &bias))
                return false;
        if (t < data->t_hit) {
                data->t_hit = t;
                data->inters->face = face;
                data->inters->barycent = b;
        }
        return true;
}

static bool __raytracer_triangle_occlude(int i, struct tintersect_data* data)
{
        int* face = geomcache_index_at(data->aos, i*3);
        struct point3d* p0 = geomcache_vertex_at(data->aos, face[0]);
        struct point3d* p1 = geomcache_vertex_at(data->aos, face[1]);
        struct point3d* p2 = geomcache_vertex_at(data->aos, face[2]);
        float bias;
        struct point3d b;
        return intersect_ray_triangle3d(data->ray, p0, p1, p2, (float*) &b, &data->t_hit, &bias);
}

bool raytracer_tintersect(struct raytracer* self, struct ray3d* ray, struct tintersect* inters)
{
        struct tintersect_data data;
        data.inters = inters;
        data.ray = ray;
        data.aos = &self->aos;
        data.t_hit = FLOAT_MAX;

        u_access_find(&self->acc, &data, __raytracer_simplex, __raytracer_triangle_tintersect);

        if (data.t_hit < FLOAT_MAX) {
                // initialize hitgeom when intersection if found
                struct point3d* p0, *p1, *p2;
                struct vector3d *n0, *n1, *n2;
                struct vector2d *t0, *t1, *t2;

                p0 = geomcache_vertex_at(&self->aos, inters->face[0]);
                p1 = geomcache_vertex_at(&self->aos, inters->face[1]);
                p2 = geomcache_vertex_at(&self->aos, inters->face[2]);
                struct point3d p;
                point3d_comps(p.p[i] = p0->p[i]*inters->barycent.v[0] +
                                       p1->p[i]*inters->barycent.v[1] +
                                       p2->p[i]*inters->barycent.v[2]);

                n0 = geomcache_normal_at(&self->aos, inters->face[0]);
                n1 = geomcache_normal_at(&self->aos, inters->face[1]);
                n2 = geomcache_normal_at(&self->aos, inters->face[2]);
                struct vector3d n;
                vector3d_comps(n.v[i] = n0->v[i]*inters->barycent.v[0] +
                                        n1->v[i]*inters->barycent.v[1] +
                                        n2->v[i]*inters->barycent.v[2]);
                normalize_vector3d_u(&n);

                if (geomcache_has_uv(&self->aos)) {
                        t0 = geomcache_uv_at(&self->aos, inters->face[0]);
                        t1 = geomcache_uv_at(&self->aos, inters->face[1]);
                        t2 = geomcache_uv_at(&self->aos, inters->face[2]);

                        struct vector2d uv;
                        vector2d_comps(uv.v[i] = t0->v[i]*inters->barycent.v[0] +
                                                 t1->v[i]*inters->barycent.v[1] +
                                                 t2->v[i]*inters->barycent.v[2]);

                        hitgeom_init(&inters->geom, &p, &n, &uv);
                } else {
                        hitgeom_init(&inters->geom, &p, &n, nullptr);
                }
                return true;
        } else {
                return false;
        }
}

bool raytracer_occlusion(struct raytracer* self, struct ray3d* ray)
{
        struct tintersect_data data;
        data.inters = nullptr;
        data.ray = ray;
        data.aos = &self->aos;
        data.t_hit = FLOAT_MAX;

        u_access_find2(&self->acc, &data, __raytracer_simplex, __raytracer_triangle_occlude);
        return data.t_hit < FLOAT_MAX;
}

/*
 * <raytracer> test cases
 */
static void __generate_random_triangle(struct triangle3d* tri, float scale)
{
        struct point3d p0, p1, p2;
        point3d_comps(p0.p[i] = uniform0_1()*2.0*scale - scale);
        point3d_comps(p1.p[i] = uniform0_1()*2.0*scale - scale);
        point3d_comps(p2.p[i] = uniform0_1()*2.0*scale - scale);
        build_triangle3d(&p0, &p1, &p2, tri);
}

static void __generate_geometries(struct triangle3d* tri, struct geomcache* gc, struct spatial_linear* acc)
{
        struct vector3d n;
        triangle3d_normal(tri, &n);

        struct vector3d normals[3];
        int i;
        for (i = 0; i < 3; i ++) {
                normals[i] = n;
        }
        int indices[3] = {0, 1, 2};
        geomcache_init(gc, UtilAttriVertex | UtilAttriNormal);
        geomcache_accumulate(gc, indices, 3, 3, tri->v, normals);

        static struct box3d simplex;
        init_box3d(&simplex);
        for (i = 0; i < 3; i ++) {
                union_box3d_p(&simplex, &tri->v[i], &simplex);
        }

        u_linear_init(acc, &simplex, 1);
        u_access_build(&acc->_parent);
}

static void __generate_random_barycentric(float b[3])
{
        int o[3] = {0, 1, 2};
        shuffle(o, 3);
        b[o[0]] = uniform0_1e();
        b[o[1]] = (1.0 - b[o[0]])*uniform0_1e();
        b[o[2]] = (1.0 - b[o[0]] - b[o[1]]);
}

static bool __is_same_coordinate(float b0[3], float b1[3])
{
        // if b0 is a subset of b1
        int k;
        for (k = 0; k < 3; k ++) {
                int l;
                for (l = 0; l < 3; l ++) {
                        if (fcmp_absp(b0[k], b1[l], EPSILON_E4)) {
                                break;
                        }
                }
                if (l == 3) {
                        return false;
                }
        }
        // if b1 is a subset of b0
        for (k = 0; k < 3; k ++) {
                int l;
                for (l = 0; l < 3; l ++) {
                        if (fcmp_absp(b1[k], b0[l], EPSILON_E4)) {
                                break;
                        }
                }
                if (l == 3) {
                        return false;
                }
        }
        return true;
}

void raytracer_test_init(struct alg_var_set* envir) {};
void raytracer_test_free(struct alg_var_set* envir) {};
enum DebugPosition* raytracer_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
void raytracer_test(struct alg_var_set* envir)
{
        const int c_NumTriangleSample = 100;
        const int c_NumIntersectionSample = 100;

        int i;
        for (i = 0; i < c_NumTriangleSample; i ++) {
                // generate a random triangle
                const float c_WorldScale = 500.0f;
                struct triangle3d tri;
                __generate_random_triangle(&tri, c_WorldScale);

                struct geomcache gc;
                struct spatial_linear acc;
                __generate_geometries(&tri, &gc, &acc);

                struct raytracer rt;
                raytracer_init(&rt, &gc, &acc._parent);
                // intersect at random points inside the triangle
                int j;
                for (j = 0; j < c_NumIntersectionSample; j ++) {
                        float bary[3];
                        __generate_random_barycentric(bary);

                        struct ray3d ray;
                        struct point3d p0 = {0.0f, 0.0f, 0.0f}, p1;
                        point3d_comps(p1.p[i] = tri.v[0].p[i]*bary[0] +
                                                tri.v[1].p[i]*bary[1] +
                                                tri.v[2].p[i]*bary[2]);
                        ray3d_build_t(&ray, &p0, &p1, 0.0f);

                        struct tintersect inters;
                        if (!raytracer_tintersect(&rt, &ray, &inters)) {
                                log_severe_err("%d failed to intersect a point inside the triangle", i);
                        }
                        // test if the barycentric coordinate is the same
                        if (!__is_same_coordinate(bary, inters.barycent.v)) {
                                log_severe_err("%d failed to intersect at the right point", i);
                        }
                }
                raytracer_free(&rt);
                geomcache_free(&gc);
                u_access_free(&acc._parent);
        }
}
