#ifndef PROJ_PROBE_H_INCLUDED
#define PROJ_PROBE_H_INCLUDED


#include <math/math.h>

enum PROJ_PROBE_IDR {
	PERSPECTIVE_PROBE,
	ORTHOGONAL_PROBE,
	FISHEYE_PROBE,
	SPHERICAL_PROBE
};

/*
 * structures
 */
struct proj_probe {
	enum PROJ_PROBE_IDR idr;
	void *probe;
};

/*
 * functions' declaration
 */
void init_proj_probe ( enum PROJ_PROBE_IDR idr, struct proj_probe *probe );
void free_proj_probe ( struct proj_probe *probe );
void proj_probe_pos ( struct point3d *pos, struct proj_probe *probe );
void proj_probe_base ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
		       struct proj_probe *probe );
void proj_probe_dir ( struct vector3d *up, struct vector3d *target,
		      struct proj_probe *probe );
void proj_probe_optics ( float focal_len, float v_dist, float apert, struct proj_probe *probe );
void proj_probe_range ( float x, float y, float z, struct proj_probe *probe );
void proj_probe_get_transform ( struct proj_probe *probe,
				struct matrix4x4 *view, struct matrix4x4 *proj );
void proj_probe_emit_ray ( float ndc_x, float ndc_y, struct proj_probe *probe, struct line3d *ray );


#endif // PROJ_PROBE_H_INCLUDED
