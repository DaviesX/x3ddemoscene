#ifndef PROJ_PROBE_H_INCLUDED
#define PROJ_PROBE_H_INCLUDED


#include <math/math.h>

struct fbo;

enum PROJ_PROBE_IDR {
        PERSPECTIVE_PROBE,
        ORTHOGONAL_PROBE,
        FISHEYE_PROBE,
        SPHERICAL_PROBE
};

enum DisplayType {
        DisplayNull,
        DisplayGtk,
        DisplayGtkOpenGL
};

/*
 * structures
 */
struct proj_probe {
        enum PROJ_PROBE_IDR     idr;
        void*                   probe;
        enum DisplayType        dis_type;
        void*                   widget;
        struct fbo*             proj_data;
        int                     xres;
        int                     yres;
        int                     res_size;
};

/*
 * functions' declaration
 */
void init_proj_probe(enum PROJ_PROBE_IDR idr, struct proj_probe *probe);
void free_proj_probe(struct proj_probe *probe);
void proj_probe_init(struct proj_probe* probe, enum PROJ_PROBE_IDR idr,
                     int xres, int yres, int bits,
                     enum DisplayType display_type, void* widget);
void proj_probe_free(struct proj_probe* probe);
void proj_probe_pos(struct point3d *pos, struct proj_probe *probe);
void proj_probe_base(struct vector3d *u, struct vector3d *v, struct vector3d *n,
                     struct proj_probe *probe);
void proj_probe_dir(struct vector3d *up, struct vector3d *target,
                    struct proj_probe *probe);
void proj_probe_optics(float focal_len, float v_dist, float apert, struct proj_probe *probe);
void proj_probe_range(float x, float y, float z, struct proj_probe *probe);
void proj_probe_get_transform(struct proj_probe *probe,
                              struct matrix4x4 *view, struct matrix4x4 *proj);
void proj_probe_emit_ray(float ndc_x, float ndc_y, struct proj_probe *probe, struct line3d *ray);

void proj_probe_take_this_fbo(struct proj_probe* probe, struct fbo* fbo);

void proj_probe_display(struct proj_probe* probe, void* widget);
void proj_probe_save_to_file(struct proj_probe* probe, char* filename);

int proj_probe_get_xres(struct proj_probe* probe);
int proj_probe_get_yres(struct proj_probe* probe);
int proj_probe_get_bits(struct proj_probe* probe);


#endif // PROJ_PROBE_H_INCLUDED
