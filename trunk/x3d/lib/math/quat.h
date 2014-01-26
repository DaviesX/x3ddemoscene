#ifndef QUAT_H_INCLUDED
#define QUAT_H_INCLUDED

/*
 * Structures
 */

struct quat {
	union {
		float m[4];
		struct {
			float  qr;			/* the real part */
			struct vector3d qi;		/* the imaginary part i*x + j*y + k*z */
		};
		struct {
			float w;
			float x, y, z;
		};
	};
};



/*
 * Functions definition
 */

void clear_quad ( struct quat *q );
void init_quat ( float w, float x, float y, float z, struct quat *q );
void init_quat_vector3d ( struct quat *v, struct quat *q );
void copy_quat ( struct quat *q0, struct quat *q1 );
void add_quat ( struct quat *q0, struct quat *q1, struct quat *qr );
void sub_quat ( struct quat *q0, struct quat *q1, struct quat *qr );
void mul_quat ( struct quat *q0, struct quat *q1, struct quat *qr );
void eul_angle_to_quat ( float x, float y, float z, struct quat *qr );
void rotate_point3d_quat ( struct point3d *p0, struct quat *qr, struct point3d *pr );


#endif // QUAT_H_INCLUDED
