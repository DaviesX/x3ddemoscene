#ifndef PROJECTIONPROBE_H_INCLUDED
#define PROJECTIONPROBE_H_INCLUDED

#include <math/math.h>


enum ProjectionProbeType {
        PerspectiveProbe,
        OrthogonalProbe,
        FisheyeProbe,
        SphericalProbe
};

enum OutputMethod {
        GtkRenderRegionOutput,
        GtkOpenGLOutput,
        PPMImageOutput
};

enum ColorMode {
        Color16AMode,
        Color16BMode,
        Color24Mode,
        Color32Mode,
        ColorHDR32Mode
};

struct projection_probe;
void            projprobe_free(struct projection_probe* self);
enum ProjectionProbeType projprobe_get_type(struct projection_probe* self);
int             projprobe_get_output_method(struct projection_probe* self);
void*           projprobe_get_target_screen(struct projection_probe* self);
void            projprobe_set_output_method(struct projection_probe* self, enum OutputMethod method, void* target_screen);
void            projprobe_set_output_format(struct projection_probe* self, int width, int height, enum ColorMode mode);
void            projprobe_toggle_fullscreen(struct projection_probe* self, bool on);
int             projprobe_get_width(struct projection_probe* self);
int             projprobe_get_height(struct projection_probe* self);
enum ColorMode  projprobe_get_colormode(struct projection_probe* self);
bool            projprobe_is_fullscreen(struct projection_probe* self);
void            projprobe_rotate_relative(struct projection_probe* self, float x, float y, float z);
void            projprobe_move_relative(struct projection_probe* self, float x);
void            projprobe_rotate(struct projection_probe* self, float x, float y, float z);
void            projprobe_move(struct projection_probe* self, float x);
void            projprobe_transform_base(struct projection_probe* self, struct matrix4x4* transform);
void            projprobe_transform_base_relative(struct projection_probe* self, struct matrix4x4* transform);
void            projprobe_set_position(struct projection_probe* self, struct point3d* p);
void            projprobe_set_base(struct projection_probe* self, struct vector3d* u, struct vector3d* v, struct vector3d* n);
void            projprobe_get_position(struct projection_probe* self, struct point3d* p);
void            projprobe_get_base(struct projection_probe* self, struct vector3d* u, struct vector3d* v, struct vector3d* n);

struct perspective_probe;
struct perspective_probe*       persprobe_create(enum OutputMethod method, void* target_screen);
void                            persprobe_set_optics(struct perspective_probe* self, float focal_len, float u_dist, float apert);
void                            persprobe_set_direction(struct perspective_probe* self,
                                                        struct vector3d* up, struct point3d* target);
void                            persprobe_set_range(struct perspective_probe* self, float x, float y, float z);
void                            persprobe_get_transform(struct perspective_probe* self,
                                                        struct matrix4x4* t_view,
                                                        struct matrix4x4* t_proj);
void                            persprobe_clone(struct perspective_probe* self, struct perspective_probe* probe);

struct orthogonal_probe;
struct orthogonal_probe*        orthoprobe_create(enum OutputMethod method, void* target_screen);

#endif // PROJECTIONPROBE_H_INCLUDED
