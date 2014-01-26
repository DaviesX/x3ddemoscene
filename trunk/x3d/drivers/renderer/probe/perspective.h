#ifndef PERSPECTIVE_H_INCLUDED
#define PERSPECTIVE_H_INCLUDED

#include <math/math.h>

struct persp_probe;

/*
 * Functions' declaration
 */

struct persp_probe *create_perspective ( void );
void free_perspective ( struct persp_probe *probe );
void perspective_set_optics ( float focal_len, float v_dist, float apert,
			      struct persp_probe *probe );
void perspective_set_pos ( struct point3d *pos, struct persp_probe *probe );
void perspective_set_base ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
			    struct persp_probe *probe );
void perspective_set_dir ( struct vector3d *up, struct point3d *target,
			   struct persp_probe *probe );
void perspective_set_range ( float x, float y, float z, struct persp_probe *probe );
void perspective_get_trans ( struct persp_probe *probe,
			     struct matrix4x4 *t_view,
			     struct matrix4x4 *t_proj );
void perspective_emit_ray ( float ndc_x, float ndc_y, struct persp_probe *probe,
			    struct line3d *ray );


#endif // PERSPECTIVE_H_INCLUDED
