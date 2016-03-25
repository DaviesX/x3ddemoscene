#include <math/math.h>
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
        int* face = geomcache_index_at(data->aos, i);
        struct point3d* p0 = geomcache_vertex_at(data->aos, face[0]);
        struct point3d* p1 = geomcache_vertex_at(data->aos, face[1]);
        struct point3d* p2 = geomcache_vertex_at(data->aos, face[2]);
        float bias, t;
        struct point3d b;
        if (!intersect_ray_triangle3d(data->ray, p0, p1, p2, (float*) &b, &t, &bias))
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
        int* face = geomcache_index_at(data->aos, i);
        struct point3d* p0 = geomcache_vertex_at(data->aos, face[0]);
        struct point3d* p1 = geomcache_vertex_at(data->aos, face[1]);
        struct point3d* p2 = geomcache_vertex_at(data->aos, face[2]);
        float bias, t;
        struct point3d b;
        return intersect_ray_triangle3d(data->ray, p0, p1, p2, (float*) &b, &t, &bias);
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
