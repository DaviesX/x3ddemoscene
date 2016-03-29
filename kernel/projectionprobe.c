#include <system/allocator.h>
#include <system/log.h>
#include <x3d/projectionprobe.h>


static void persprobe_update(struct perspective_probe* self);
static void persprobe_free(struct perspective_probe* self);

/* probe parent */
void projprobe_init(struct projection_probe* self, enum ProjectionProbeType type,
                    struct display* display, int xres, int yres,
                    f_Projprobe_Sample_At f_sample, f_Projprobe_Free f_free)
{
        self->type              = type;
        self->w                 = xres;
        self->h                 = yres;
        set_point3d(0.0f, 0.0f, 0.0f, &self->pos);
        set_vector3d(0.0f, 0.0f, 1.0f, &self->u);
        set_vector3d(0.0f, 1.0f, 0.0f, &self->v);
        set_vector3d(1.0f, 0.0f, 0.0f, &self->n);
        self->display = display;
        self->f_sample = f_sample;
        self->f_free = f_free;
}

void projprobe_free(struct projection_probe* self)
{
        self->f_free(self);
}

enum ProjectionProbeType projprobe_get_type(struct projection_probe* self)
{
        return self->type;
}

int projprobe_get_width(struct projection_probe* self)
{
        return self->w;
}

int projprobe_get_height(struct projection_probe* self)
{
        return self->h;
}

void projprobe_rotate(struct projection_probe* self, float x, float y, float z)
{
}

void projprobe_move(struct projection_probe* self, float x)
{
}

void projprobe_rotate_relative(struct projection_probe* self, float x, float y, float z)
{
}

void projprobe_move_relative(struct projection_probe* self, float x)
{
}

void projprobe_transform_base(struct projection_probe* self, struct matrix4x4* transform)
{
}

void projprobe_transform_base_relative(struct projection_probe* self, struct matrix4x4* transform)
{
}

void projprobe_set_position(struct projection_probe* self, struct point3d* p)
{
        self->backup_pos = *p;
        self->pos        = *p;
}

void projprobe_set_base(struct projection_probe* self, struct vector3d* u, struct vector3d* v, struct vector3d* n)
{
        self->backup_u = *u;
        self->backup_v = *v;
        self->backup_n = *n;

        self->u = *u;
        self->v = *v;
        self->n = *n;
}

void projprobe_get_position(struct projection_probe* self, struct point3d* p)
{
        *p = self->pos;
}

void projprobe_get_base(struct projection_probe* self, struct vector3d* u, struct vector3d* v, struct vector3d* n)
{
        *u = self->u;
        *v = self->v;
        *n = self->n;
}

void projprobe_sample_at(struct projection_probe* self, int i, int j, struct ray3d* ray)
{
        self->f_sample(self, i, j, ray);
}

struct display* projprobe_get_display(struct projection_probe* self)
{
        return self->display;
}

/* perspective probe */
struct perspective_probe* persprobe_create(struct display* display, int xres, int yres)
{
        struct perspective_probe *self = alloc_obj(self);
        projprobe_init(&self->_parent, PerspectiveProbe, display, xres, yres,
                       (f_Projprobe_Sample_At) persprobe_sample_at,
                       (f_Projprobe_Free) persprobe_free);
        return self;
}

static void persprobe_free(struct perspective_probe* self)
{
        zero_obj(self);
}

void persprobe_set_optics(struct perspective_probe* self, float focal_len, float u_dist, float apert)
{
        if (fabsf(focal_len) < 1e-1f) {
                focal_len = 1e-1f;
                log_mild_err_dbg("focal length is too small, it will be fixed at %f mm", focal_len);
        }
        focal_len       = mm_to_unit(focal_len);
        self->f_dist    = focal_len;
        float v_dist    = focal_len*u_dist/(u_dist - focal_len);
        if (fabsf(v_dist - focal_len) < 1e-3f) {
                v_dist = focal_len + 1e-3f;
                u_dist = focal_len*v_dist/(v_dist - focal_len);
                log_mild_err_dbg("v distance is too closed to focal length, it will be fixed at %f m", v_dist);
        }
        self->v_dist    = v_dist;
        self->u_dist    = u_dist;
        self->tan_hfov  = 0.5f/v_dist;
        apert           = mm_to_unit(apert);
        self->apert     = apert;
        if (apert != 0) {
                self->tan_hfov = min(self->tan_hfov, apert/focal_len);
        }
        self->nz = 0.5f/self->tan_hfov;
}

void persprobe_set_direction(struct perspective_probe* self, struct vector3d* up, struct point3d* target)
{
        struct point3d p;
        struct vector3d u, v, n;
        projprobe_get_position((struct projection_probe*) self, &p);
        build_vector3d(&p, target, &n);
        cross_vector3d(up, &n, &u);
        cross_vector3d(&n, &u, &v);
        projprobe_set_base((struct projection_probe*) self, &u, &v, &n);
}

void persprobe_set_range(struct perspective_probe* self, float x, float y, float z)
{
        self->plate_ratio       = x/y;
        self->fz                = z;
}

static void persprobe_update(struct perspective_probe* self)
{
        struct point3d pos;
        projprobe_get_position((struct projection_probe*) self, &pos);
        struct vector3d u, v, n;
        projprobe_get_base((struct projection_probe*) self, &u, &v, &n);

        /* t_view */
        struct matrix4x4 t;
        struct matrix4x4 r;
        get_translate_matrix4x4(pos.x, pos.y, pos.z, &t);
        get_rotate_to_uvn_matrix4x4(&u, &v, &n, &r);
        mul_matrix4x4(&r, &t, &self->t_view);

        /* t_proj: (NL*x, NT*y, az + b, z)
         * x' = nx/z * 1/R (R = 1/2*len)
         * y' = ny/z * r/R (r = len/height)
         * z' = 1/z * [(f + n)/(f - n)z + 2fn/(n - f)] */
        const float inv_R       = 1.0f;	/* R = 1.0f/2.0f*2.0f */
        float nl                = self->nz * inv_R;
        float nt                = self->nz * self->plate_ratio*inv_R;
        float a                 = (self->nz + self->fz)/(self->fz - self->nz);
        float b                 = 2.0f*self->nz*self->fz/(self->nz - self->fz);
        set_matrix4x4(&self->t_proj,
                      nl, 0.0f, 0.0f, 0.0f,
                      0.0f, nt, 0.0f, 0.0f,
                      0.0f, 0.0f, a,  1.0f,
                      0.0f, 0.0f, b,  0.0f);

        /* Inverse process */
        inv_matrix4x4(&self->t_view, &self->inv_t_view);
        inv_matrix4x4(&self->t_proj, &self->inv_t_proj);
        mul_matrix4x4(&self->inv_t_view, &self->inv_t_proj, &self->t_inv_all);
}

void persprobe_get_transform(struct perspective_probe* self, struct matrix4x4* t_view, struct matrix4x4* t_proj)
{
        persprobe_update(self);
        self->t_view = *t_view;
        self->t_proj = *t_proj;
}

void persprobe_clone(struct perspective_probe* self, struct perspective_probe* probe)
{
        *probe = *self;
}

void persprobe_sample_at(struct perspective_probe* self, int i, int j, struct ray3d* ray)
{
        struct point3d p0;
        projprobe_get_position(&self->_parent, &p0);

        float x = (float) i/(projprobe_get_width(&self->_parent) - 1) - 0.5f;
        float y = 0.5f - (float) j/(projprobe_get_height(&self->_parent) - 1);

        struct vector3d v = {x, y/self->plate_ratio, self->v_dist};
        float len = length_vector3d(&v);
        vector3d_comps(v.v[i] = v.v[i]/len);
        ray3d_build_t3(ray, &p0, &v, len, FLOAT_MAX);
}

/* orthogonal probe */
struct orthogonal_probe* orthoprobe_create(struct display* display, int xres, int yres)
{
        struct orthogonal_probe* self = alloc_obj(self);
        projprobe_init(&self->_parent, OrthogonalProbe, display, xres, yres,
                       (f_Projprobe_Sample_At) orthoprobe_sample_at,
                       (f_Projprobe_Free) orthoprobe_free);
        return self;
}

void orthoprobe_free(struct orthogonal_probe* self)
{
}

void orthoprobe_sample_at(struct orthogonal_probe* self, int i, int j, struct ray3d* ray)
{
        log_critical_err("not implemenetd yet");
}
