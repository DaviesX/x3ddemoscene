#include <math/math.h>
#include <x3d/raytracer.h>


void raytracer_init(struct raytracer* self,
                    struct util_aos* aos,
                    struct util_access* acc)
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
        struct util_aos*        aos;
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
        int* face = u_aos_index_at(data->aos, i);
        struct point3d* p0 = u_aos_vertex_at(data->aos, face[0]);
        struct point3d* p1 = u_aos_vertex_at(data->aos, face[1]);
        struct point3d* p2 = u_aos_vertex_at(data->aos, face[2]);
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
        int* face = u_aos_index_at(data->aos, i);
        struct point3d* p0 = u_aos_vertex_at(data->aos, face[0]);
        struct point3d* p1 = u_aos_vertex_at(data->aos, face[1]);
        struct point3d* p2 = u_aos_vertex_at(data->aos, face[2]);
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
        return data.t_hit < FLOAT_MAX;
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
