#include <math/math.h>
#include <system/allocator.h>
#include <x3d/colorspectrum.h>
#include <x3d/light.h>


/*
 * <light> public
 */
void light_init(struct light* self, f_Light_Sample_At f_sample, f_Light_Sample_At2 f_sample2, f_Light_Free f_free)
{
        self->f_sample = f_sample;
        self->f_sample2 = f_sample2;
        self->f_free = f_free;
}

void light_free(struct light* self)
{
        self->f_free(self);
}

void light_sample_at(struct light* self, struct point3d* p, struct vector3d* n, struct float_color3* i)
{
        self->f_sample(self, p, n, i);
}

float light_sample_at2(struct light* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i)
{
        return self->f_sample2(self, p0, illumray, i);
}


/*
 * <light_point> public
 */
struct light_point* light_point_create(struct float_color3* flux, struct point3d* p, float radius)
{
        struct light_point* self = alloc_obj(self);
        light_init(&self->_parent,
                   (f_Light_Sample_At)  light_point_sample_at,
                   (f_Light_Sample_At2) light_point_sample_at2,
                   (f_Light_Free)       light_point_free);
        scale_color3(1/(2.0*M_PI*4.0*M_PI*radius*radius), flux, &self->inten);  // intensity at a point in any direction
        self->center = *p;
        self->radius = radius;
        self->half_r2 = radius*radius/2.0f;
        self->flux = *flux;
        return self;
}

void light_point_free(struct light_point* self)
{
}

void light_point_sample_at(struct light_point* self, struct point3d* p, struct vector3d* n, struct float_color3* i)
{
        // imagine we are sampling on a sphere
        struct spherical3d sp;
        sp.the = uniform0_1()*M_PI;
        sp.phi = uniform0_1()*M_PI - M_PI_2;
        sp.r = 1.0f;
        spherical_to_vector3d(&sp, n);
        scale_vector3d(self->radius, n, p);
        add_point3d_u(p, &self->center);
        *i = self->inten;
}

float light_point_sample_at2(struct light_point* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i)
{
        // imagine we are sampling on a sphere
        struct spherical3d sp;
        sp.the = uniform0_1()*M_PI;
        sp.phi = uniform0_1()*M_PI - M_PI_2;
        sp.r = 1.0f;
        struct point3d p;
        struct vector3d n;
        spherical_to_vector3d(&sp, &n);
        scale_vector3d(self->radius, &n, &p);
        add_point3d_u(&p, &self->center);
        ray3d_build_t(illumray, p0, &p, 0.0, FLOAT_MAX);

        float cos = -1.0*dot_vector3d(&n, &illumray->v);        // the ray is inverted
        if (cos > 0.0f) {
                // P(get hit) = (PI*r^2)/(2*PI*dist^2)
                float p = self->half_r2*ray3d_inv_length2(illumray);
                scale_color3(cos*p, &self->inten, i);
                return p;
        } else {
                init_color3(0.0f, i);
                return 0.0f;
        }
}

/*
 * <light_rectangular> public
 */
struct light_rectangular* light_rect_create(struct float_color3* flux,
                struct point3d* p0, struct point3d* p1, struct point3d* p2, struct point3d* p3)
{
        struct light_rectangular* self = alloc_obj(self);
        light_init(&self->_parent,
                   (f_Light_Sample_At)  light_rect_sample_at,
                   (f_Light_Sample_At2) light_rect_sample_at2,
                   (f_Light_Free)       light_rect_free);
        self->p0 = *p0;
        self->p1 = *p1;
        self->p2 = *p2;
        self->p3 = *p3;
        struct triangle3d tri;
        build_triangle3d(p0, p1, p2, &tri);
        triangle3d_normal(&tri, &self->n);
        self->area = surface_area_triangle3d(&tri);
        scale_color3(1/(self->area*2*M_PI), flux, &self->inten);
        self->flux = *flux;
        return self;
}

void light_rect_free(struct light_rectangular* self)
{
}

static void __random_barycentric(float x[4])
{
        int o[4] = {0, 1, 2, 3};
        shuffle(o, 4);
        x[o[0]] = uniform0_1();
        x[o[1]] = (1 - x[o[0]])*uniform0_1();
        x[o[2]] = (1 - x[o[0]] - x[o[1]])*uniform0_1();
        x[o[3]] = (1 - x[o[0]] - x[o[1]] - x[o[2]]);
}


void light_rect_sample_at(struct light_rectangular* self, struct point3d* p, struct vector3d* n, struct float_color3* i)
{
        float b[4];
        __random_barycentric(b);
        point3d_comps(p->p[i] = self->p0.p[i]*b[0] +
                                self->p1.p[i]*b[1] +
                                self->p2.p[i]*b[2] +
                                self->p3.p[i]*b[3]);
        *n = self->n;
        *i = self->inten;
}

float light_rect_sample_at2(struct light_rectangular* self, struct point3d* p0, struct ray3d* illumray, struct float_color3* i)
{
        float b[4];
        __random_barycentric(b);
        struct point3d p;
        point3d_comps(p.p[i] = self->p0.p[i]*b[0] +
                               self->p1.p[i]*b[1] +
                               self->p2.p[i]*b[2] +
                               self->p3.p[i]*b[3]);
        ray3d_build_t(illumray, p0, &p, 0.0, FLOAT_MAX);

        float cos = -1.0*dot_vector3d(&self->n, &illumray->v);  // the ray is inverted
        if (cos > 0.0f) {
                // P(get hit) = A*cos(t)/(2*PI*dist^2)
                float p = self->area*cos/(2.0f*M_PI)*ray3d_inv_length2(illumray);
                scale_color3(cos*p, &self->inten, i);
                return p;
        } else {
                init_color3(0.0f, i);
                return 0.0f;
        }
}
