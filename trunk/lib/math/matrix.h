#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED


#include <x3d/common.h>

/*
 * Structures
 */

struct matrix2x2 {
        union {
                float m[2][2];	// array type;

                struct {
                        float m00, m01;	// variable type
                        float m10, m11;
                };
        };
};

struct matrix3x3 {
        union {
                float m[3][3];	// array type

                struct {
                        float m00, m01, m02;	// variable type
                        float m10, m11, m12;
                        float m20, m21, m22;
                };
        };
};

struct matrix4x4 {
        union {
                float m[4][4];	// array type

                struct {
                        float m00, m01, m02, m03;	// variable type
                        float m10, m11, m12, m13;
                        float m20, m21, m22, m23;
                        float m30, m31, m32, m33;
                };
        };
};

struct matrix1x2 {
        union {
                float m[2];	// array type

                struct {
                        float m00, m01;	// variable type
                };
        };
};

struct matrix1x3 {
        union {
                float m[3];	// array type

                struct {
                        float m00, m01, m02;	// variable type
                };
        };
};

struct matrix1x4 {
        union {
                float m[4];	// array type

                struct {
                        float m00, m01, m02, m03;	// variable type
                };
        };
};

struct matrix4x3 {
        union {
                float m[4][3];	// array indexed type

                struct {
                        float m00, m01, m02;	// variable type
                        float m10, m11, m12;
                        float m20, m21, m22;
                        float m30, m31, m32;
                };
        };
};

struct matrix3x2 {
        union {
                float m[3][2]; // array indexed data storage

                struct {
                        float m00, m01;	// variable type
                        float m10, m11;
                        float m20, m21;
                };
        };
};



/*
 * Constants
 */

extern const struct matrix4x4 IdentityMatrix4x4;
extern const struct matrix4x3 IdentityMatrix4x3;
extern const struct matrix3x3 IdentityMatrix3x3;
extern const struct matrix3x2 IdentityMatrix3x2;
extern const struct matrix2x2 IdentityMatrix2x2;


/*
 * Definitions
 */

/* Clear out the Matrices */
#define clear_matrix2x2( _m ) (memset ( _m, 0, sizeof ( struct matrix2x2 ) ))
#define clear_matrix3x3( _m ) (memset ( _m, 0, sizeof ( struct matrix3x3 ) ))
#define clear_matrix4x4( _m ) (memset ( _m, 0, sizeof ( struct matrix4x4 ) ))
#define clear_matrix4x3( _m ) (memset ( _m, 0, sizeof ( struct matrix4x3 ) ))
#define clear_matrix3x2( _m ) (memset ( _m, 0, sizeof ( struct matrix3x2 ) ))
#define clear_matrix1x4( _m ) (memset ( _m, 0, sizeof ( struct matrix1x4 ) ))
#define clear_matrix1x3( _m ) (memset ( _m, 0, sizeof ( struct matrix1x3 ) ))
#define clear_matrix1x2( _m ) (memset ( _m, 0, sizeof ( struct matrix1x2 ) ))

/* Set identity matrix */
#define identity_matrix2x2( _m ) (memcpy ( _m, &IdentityMatrix2x2, sizeof ( struct matrix2x2 ) ))
#define identity_matrix3x3( _m ) (memcpy ( _m, &IdentityMatrix3x3, sizeof ( struct matrix3x3 ) ))
#define identity_matrix4x4( _m ) (memcpy ( _m, &IdentityMatrix4x4, sizeof ( struct matrix4x4 ) ))
#define identity_matrix4x3( _m ) (memcpy ( _m, &IdentityMatrix4x3, sizeof ( struct matrix4x3 ) ))
#define identity_matrix3x2( _m ) (memcpy ( _m, &IdentityMatrix3x2, sizeof ( struct matrix3x2 ) ))

/* Copy the matrices */
#define copy_matrix2x2(_src, _dest) (memcpy ( _dest, _src, sizeof ( struct matrix2x2 ) ))
#define copy_matrix3x3(_src, _dest) (memcpy ( _dest, _src, sizeof ( struct matrix3x3 ) ))
#define copy_matrix4x4(_src, _dest) (memcpy ( _dest, _src, sizeof ( struct matrix4x4 ) ))
#define copy_matrix4x3(_src, _dest) (memcpy ( _dest, _src, sizeof ( struct matrix4x3 ) ))
#define copy_matrix3x2(_src, _dest) (memcpy ( _dest, _src, sizeof ( struct matrix3x2 ) ))



/*
 * Functions defintion
 */

void set_matrix2x2 ( struct matrix2x2 *matrix,
                     float m00, float m01,
                     float m10, float m11 );
void set_matrix3x3 ( struct matrix3x3 *matrix,
                     float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22 );
void set_matrix3x2 ( struct matrix3x2 *matrix,
                     float m00, float m01,
                     float m10, float m11,
                     float m20, float m21 );
void set_matrix4x4 ( struct matrix4x4 * matrix,
                     float m00, float m01, float m02, float m03,
                     float m10, float m11, float m12, float m13,
                     float m20, float m21, float m22, float m23,
                     float m30, float m31, float m32, float m33 );
void set_matrix4x3 ( struct matrix4x3 * matrix,
                     float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22,
                     float m30, float m31, float m32 );

void transpose_matrix2x2 ( struct matrix2x2 *m_in, struct matrix2x2 *m_out );
void transpose_matrix3x3 ( struct matrix3x3 *m_in, struct matrix3x3 *m_out );
void transpose_matrix4x4 ( struct matrix4x4 *m_in, struct matrix4x4 *m_out );

void add_matrix2x2 ( struct matrix2x2 *matrix0, struct matrix2x2 *matrix1, struct matrix2x2 *m_out );
void add_matrix3x3 ( struct matrix3x3 *matrix0, struct matrix3x3 *matrix1, struct matrix3x3 *m_out );
void add_matrix3x2 ( struct matrix3x2 *matrix0, struct matrix3x2 *matrix2, struct matrix3x2 *m_out );
void add_matrix4x4 ( struct matrix4x4 *matrix0, struct matrix4x4 *matrix1, struct matrix4x4 *m_out );
void add_matrix4x3 ( struct matrix4x3 *matrix0, struct matrix4x3 *matrix1, struct matrix4x3 *m_out );

void scale_matrix2x2 ( float k, struct matrix2x2 *matrix, struct matrix2x2 *m_out );
void scale_matrix3x3 ( float k, struct matrix3x3 *matrix, struct matrix3x3 *m_out );
void scale_matrix3x2 ( float k, struct matrix3x2 *matrix, struct matrix3x2 *m_out );
void scale_matrix4x4 ( float k, struct matrix4x4 *matrix, struct matrix4x4 *m_out );
void scale_matrix4x3 ( float k, struct matrix4x3 *matrix, struct matrix4x3 *m_out );

void mul_matrix2x2 ( struct matrix2x2 *matrix0, struct matrix2x2 *matrix1, struct matrix2x2 *m_out );
void mul_matrix3x3 ( struct matrix3x3 *matrix0, struct matrix3x3 *matrix1, struct matrix3x3 *m_out );
void mul_matrix4x4 ( struct matrix4x4 *matrix0, struct matrix4x4 *matrix1, struct matrix4x4 *m_out );

void mul_point3d_matrix3x3 ( struct point3d *p, struct matrix3x3 *matrix, struct point3d *p_out );
void mul_point3d_matrix4x4 ( struct point3d *p, struct matrix4x4 *matrix, struct point3d *p_out );
void mul_point3d_matrix4x3 ( struct point3d *p, struct matrix4x3 *matrix, struct point3d *p_out );
void mul_point4d_matrix4x4 ( struct point4d *p, struct matrix4x4 *matrix, struct point4d *p_out );
void mul_point4d_matrix4x3 ( struct point4d *p, struct matrix4x3 *matrix, struct point4d *p_out );

float det_matrix2x2 ( struct matrix2x2 *matrix );
float det_matrix3x3 ( struct matrix3x3 *matrix );
float det_matrix4x4 ( struct matrix4x4 *matrix );

void inv_matrix2x2 ( struct matrix2x2 *matrix, struct matrix2x2 *m_out );
void inv_matrix3x3 ( struct matrix3x3 *matrix, struct matrix3x3 *m_out );
void inv_matrix4x4 ( struct matrix4x4 *matrix, struct matrix4x4 *m_out );

int solve_2x2_cramer_s ( struct matrix2x2 *coe, struct vector2d *c, float *x, float *y );
void solve_2x2_cramer ( struct matrix2x2 *coe, struct vector2d *c, float *x, float *y );
int solve_3x3_cramer_s ( struct matrix3x3 *coe, struct vector3d *c, float *x, float *y, float *z );
void solve_3x3_cramer ( struct matrix3x3 *coe, struct vector3d *c, float *x, float *y, float *z );

void get_translate_matrix4x4 ( float x, float y, float z, struct matrix4x4 *m_out );
void get_rotate_x_matrix4x4 ( float euler_x, struct matrix4x4 *m_out );
void get_rotate_y_matrix4x4 ( float euler_y, struct matrix4x4 *m_out );
void get_rotate_z_matrix4x4 ( float euler_z, struct matrix4x4 *m_out );
void get_rotate_xyz_matrix4x4 ( float euler_x, float euler_y, float euler_z, struct matrix4x4 *m_out );
void get_rotate_to_uvn_matrix4x4 ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
                                   struct matrix4x4 *m_out );
void get_scale_matrix4x4 ( float x, float y, float z, struct matrix4x4 *m_out );

/* Unary operations */
#define transpose_matrix2x2_u( _m ) { \
	struct matrix2x2 _m_tmp; \
	transpose_matrix2x2 ( _m, &_m_tmp ); \
	memcpy ( _m, &_m_tmp, sizeof ( _m_tmp ) ); }

#define transpose_matrix3x3_u( _m ) { \
	struct matrix3x3 _m_tmp; \
	transpose_matrix3x3 ( _m, &_m_tmp ); \
	memcpy ( _m, &_m_tmp, sizeof ( _m_tmp ) ); }

#define transpose_matrix4x4_u( _m ) { \
	struct matrix4x4 _m_tmp; \
	transpose_matrix4x4 ( _m, &_m_tmp ); \
	memcpy ( _m, &_m_tmp, sizeof ( _m_tmp ) ); }

#define add_matrix2x2_u( _m0, _m1 )		(add_matrix2x2 ( _m0, _m1, _m0 ))
#define add_matrix3x3_u( _m0, _m1 )		(add_matrix3x3 ( _m0, _m1, _m0 ))
#define add_matrix3x2_u( _m0, _m1 )		(add_matrix3x2 ( _m0, _m1, _m0 ))
#define add_matrix4x4_u( _m0, _m1 )		(add_matrix4x4 ( _m0, _m1, _m0 ))
#define add_matrix4x3_u( _m0, _m1 )		(add_matrix4x3 ( _m0, _m1, _m0 ))

#define scale_matrix2x2_u( _m0, _m1 )		(scale_matrix2x2 ( _m0, _m1, _m0 ))
#define scale_matrix3x3_u( _m0, _m1 )		(scale_matrix3x3 ( _m0, _m1, _m0 ))
#define scale_matrix3x2_u( _m0, _m1 )		(scale_matrix3x2 ( _m0, _m1, _m0 ))
#define scale_matrix4x4_u( _m0, _m1 )		(scale_matrix4x4 ( _m0, _m1, _m0 ))
#define scale_matrix4x3_u( _m0, _m1 )		(scale_matrix4x3 ( _m0, _m1, _m0 ))

#define mul_matrix2x2_u( _m0, _m1 ) {\
	struct matrix2x2 _m_tmp; \
	mul_matrix2x2 ( _m0, _m1, &_m_tmp ); \
	memcpy ( _m0, &_m_tmp, sizeof ( _m_tmp ) ); }

#define mul_matrix3x3_u( _m0, _m1 ) {\
	struct matrix3x3 _m_tmp; \
	mul_matrix3x3 ( _m0, _m1, &_m_tmp ); \
	memcpy ( _m0, &_m_tmp, sizeof ( _m_tmp ) ); }

#define mul_matrix4x4_u( _m0, _m1 ) {\
	struct matrix4x4 _m_tmp; \
	mul_matrix4x4 ( _m0, _m1, &_m_tmp ); \
	memcpy ( _m0, &_m_tmp, sizeof ( _m_tmp ) ); }

#define mul_point3d_matrix3x3_u( _v, _m ) {\
	struct point3d _p_tmp; \
	mul_point3d_matrix3x3 ( _v, _m, &_p_tmp ); \
	memcpy ( _v, &_p_tmp, sizeof ( _p_tmp ) ); }

#define mul_point3d_matrix4x4_u( _v, _m ) {\
	struct point3d _p_tmp; \
	mul_point3d_matrix4x4 ( _v, _m, &_p_tmp ); \
	memcpy ( _v, &_p_tmp, sizeof ( _p_tmp ) ); }

#define mul_point3d_matrix4x3_u( _v, _m ) {\
	struct point3d _p_tmp; \
	mul_point3d_matrix4x3 ( _v, _m, &_p_tmp ); \
	memcpy ( _v, &_p_tmp, sizeof ( _p_tmp ) ); }

#define mul_point4d_matrix4x4_u( _v, _m ) {\
	struct point4d _p_tmp; \
	mul_point4d_matrix4x4 ( _v, _m, &_p_tmp ); \
	memcpy ( _v, &_p_tmp, sizeof ( _p_tmp ) ); }

#define mul_point4d_matrix4x3_u( _v, _m ) {\
	struct point4d _p_tmp; \
	mul_point4d_matrix4x3 ( _v, _m, &_p_tmp ); \
	memcpy ( _v, &_p_tmp, sizeof ( _p_tmp ) ); }

/* Reference name */
#define mul_vector3d_matrix3x3		mul_point3d_matrix3x3
#define mul_vector3d_matrix3x3_u	mul_point3d_matrix3x3_u


#endif // MATRIX_H_INCLUDED
