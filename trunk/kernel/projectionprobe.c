#include <allocator.h>
#include <logout.h>
#include <x3d/projectionprobe.h>


struct projection_probe {
        int                     type;
        int                     w, h;
        int                     color_mode;
        int                     output_method;
        void*                   target_screen;
        bool                    is_fullscreen;
        struct point3d          backup_pos;
        struct vector3d         backup_u, backup_v, backup_n;
        struct point3d          pos;
        struct vector3d         u, v, n;
};

struct perspective_probe {
        struct projection_probe _parent;

        float                   f_dist, u_dist, v_dist;
        float                   apert;
        float                   plate_ratio;
        float                   nz, fz;
        float                   tan_hfov;

        struct matrix4x4        t_view;
        struct matrix4x4        t_proj;
        struct matrix4x4        inv_t_view;
        struct matrix4x4        inv_t_proj;
        struct matrix4x4        t_inv_all;
};

struct orthogonal_probe {
        struct projection_probe _parent;
};


static void persprobe_update(struct perspective_probe* self);
static void persprobe_free(struct perspective_probe* self);

/* probe parent */
static void projprobe_init(struct projection_probe* self,
                           enum ProjectionProbeType type, enum OutputMethod method, void* target_screen)
{
        self->type              = type;
        self->output_method     = method;
        self->target_screen     = target_screen;
        self->w                 = 0;
        self->h                 = 0;
        self->is_fullscreen     = false;
        self->color_mode        = Color32Mode;
        set_point3d(0.0f, 0.0f, 0.0f, &self->pos);
        set_vector3d(0.0f, 0.0f, 1.0f, &self->u);
        set_vector3d(0.0f, 1.0f, 0.0f, &self->v);
        set_vector3d(1.0f, 0.0f, 0.0f, &self->n);
}

void projprobe_free(struct projection_probe* self)
{
        switch(self->type) {
        case PerspectiveProbe:
                persprobe_free((struct perspective_probe*) self);
                break;
        }
        free_fix(self);
}

enum ProjectionProbeType projprobe_get_type(struct projection_probe* self)
{
        return self->type;
}

int projprobe_get_output_method(struct projection_probe* self)
{
        return self->output_method;
}

void* projprobe_get_target_screen(struct projection_probe* self)
{
        return self->target_screen;
}

void projprobe_set_output_method(struct projection_probe* self, enum OutputMethod method, void* target_screen)
{
        self->output_method = method;
        self->target_screen = target_screen;
}

void projprobe_set_output_format(struct projection_probe* self, int width, int height, enum ColorMode mode)
{
        self->w                 = width;
        self->h                 = height;
        self->color_mode        = mode;
}

void projprobe_toggle_fullscreen(struct projection_probe* self, bool on)
{
        self->is_fullscreen = on;
}

int projprobe_get_width(struct projection_probe* self)
{
        return self->w;
}

int projprobe_get_height(struct projection_probe* self)
{
        return self->h;
}

enum ColorMode projprobe_get_colormode(struct projection_probe* self)
{
        return self->color_mode;
}

bool projprobe_is_fullscreen(struct projection_probe* self)
{
        return self->is_fullscreen;
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

/* perspective probe */
struct perspective_probe* persprobe_create(enum OutputMethod method, void* target_screen)
{
        struct perspective_probe *self = alloc_obj(self);
        projprobe_init((struct projection_probe*) self, PerspectiveProbe, method, target_screen);
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

/* orthogonal probe */
struct orthogonal_probe* orthoprobe_create(enum OutputMethod method, void* target_screen)
{
        struct orthogonal_probe* self = alloc_obj(self);
        projprobe_init((struct projection_probe*) self, OrthogonalProbe, method, target_screen);
        return self;
}
