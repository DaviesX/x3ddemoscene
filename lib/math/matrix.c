/* math.c: All matrix calculation go here */
#include "math.h"
#include "vector.h"
#include "matrix.h"


// constants of identity matrix
const struct matrix4x4 IdentityMatrix4x4 = {
        .m[0] = {1, 0, 0, 0},
        .m[1] = {0, 1, 0, 0},
        .m[2] = {0, 0, 1, 0},
        .m[3] = {0, 0, 0, 1}
};

const struct matrix4x3 IdentityMatrix4x3 = {
        .m[0] = {1, 0, 0},
        .m[1] = {0, 1, 0},
        .m[2] = {0, 0, 1},
        .m[3] = {0, 0, 0}
};

const struct matrix3x3 IdentityMatrix3x3 = {
        .m[0] = {1, 0, 0},
        .m[1] = {0, 1, 0},
        .m[2] = {0, 0, 1}
};

const struct matrix3x2 IdentityMatrix3x2 = {
        .m[0] = {1, 0},
        .m[1] = {0, 1},
        .m[2] = {0, 0}
};

const struct matrix2x2 IdentityMatrix2x2 = {
        .m[0] = {1, 0},
        .m[1] = {0, 1}
};

void set_matrix2x2 ( struct matrix2x2 *matrix,
                     float m00, float m01,
                     float m10, float m11 )
{
        matrix->m00 = m00;
        matrix->m01 = m01;

        matrix->m10 = m10;
        matrix->m11 = m11;
}

void set_matrix3x3 ( struct matrix3x3 *matrix,
                     float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22 )
{
        matrix->m00 = m00;
        matrix->m01 = m01;
        matrix->m02 = m02;

        matrix->m10 = m10;
        matrix->m11 = m11;
        matrix->m12 = m12;

        matrix->m20 = m20;
        matrix->m21 = m21;
        matrix->m22 = m22;
}

void set_matrix3x2 ( struct matrix3x2 *matrix,
                     float m00, float m01,
                     float m10, float m11,
                     float m20, float m21 )
{
        matrix->m00 = m00;
        matrix->m01 = m01;

        matrix->m10 = m10;
        matrix->m11 = m11;

        matrix->m20 = m20;
        matrix->m21 = m21;
}

void set_matrix4x4 ( struct matrix4x4 * matrix,
                     float m00, float m01, float m02, float m03,
                     float m10, float m11, float m12, float m13,
                     float m20, float m21, float m22, float m23,
                     float m30, float m31, float m32, float m33 )
{
        matrix->m00 = m00;
        matrix->m01 = m01;
        matrix->m02 = m02;
        matrix->m03 = m03;

        matrix->m10 = m10;
        matrix->m11 = m11;
        matrix->m12 = m12;
        matrix->m13 = m13;

        matrix->m20 = m20;
        matrix->m21 = m21;
        matrix->m22 = m22;
        matrix->m23 = m23;

        matrix->m30 = m30;
        matrix->m31 = m31;
        matrix->m32 = m32;
        matrix->m33 = m33;
}

void set_matrix4x3 ( struct matrix4x3 * matrix,
                     float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22,
                     float m30, float m31, float m32 )
{
        matrix->m00 = m00;
        matrix->m01 = m01;
        matrix->m02 = m02;

        matrix->m10 = m10;
        matrix->m11 = m11;
        matrix->m12 = m12;

        matrix->m20 = m20;
        matrix->m21 = m21;
        matrix->m22 = m22;

        matrix->m20 = m30;
        matrix->m21 = m31;
        matrix->m22 = m32;
}

/*
 * Transpose of a matrix
 * |00 01 02 03|           |00 10 20 30|
 * |10 11 12 13|           |01 11 21 31|
 * |20 21 22 23| transpose |02 12 22 32|
 * |30 31 32 33|           |03 13 23 33|
 * Note that the dest transposed matrix MUST be at _DIFFERENT_ location
 * from the source matrix
 */
void transpose_matrix2x2 ( struct matrix2x2 *m_in, struct matrix2x2 *m_out )
{
        m_out->m00 = m_in->m00;
        m_out->m01 = m_in->m10;

        m_out->m10 = m_in->m01;
        m_out->m11 = m_in->m11;
}

void transpose_matrix3x3 ( struct matrix3x3 *m_in, struct matrix3x3 *m_out )
{
        m_out->m00 = m_in->m00;
        m_out->m01 = m_in->m10;
        m_out->m02 = m_in->m20;

        m_out->m10 = m_in->m01;
        m_out->m11 = m_in->m11;
        m_out->m12 = m_in->m21;

        m_out->m20 = m_in->m02;
        m_out->m21 = m_in->m12;
        m_out->m22 = m_in->m22;
}

void transpose_matrix4x4 ( struct matrix4x4 *m_in, struct matrix4x4 *m_out )
{
        m_out->m00 = m_in->m00;
        m_out->m01 = m_in->m10;
        m_out->m02 = m_in->m20;
        m_out->m03 = m_in->m30;

        m_out->m10 = m_in->m01;
        m_out->m11 = m_in->m11;
        m_out->m12 = m_in->m21;
        m_out->m13 = m_in->m31;

        m_out->m20 = m_in->m02;
        m_out->m21 = m_in->m12;
        m_out->m22 = m_in->m22;
        m_out->m23 = m_in->m32;

        m_out->m30 = m_in->m03;
        m_out->m31 = m_in->m13;
        m_out->m32 = m_in->m23;
        m_out->m33 = m_in->m33;
}

void add_matrix2x2 ( struct matrix2x2 *matrix0, struct matrix2x2 *matrix1, struct matrix2x2 *m_out )
{
        m_out->m00 = matrix0->m00 + matrix1->m00;
        m_out->m01 = matrix0->m01 + matrix1->m01;

        m_out->m10 = matrix0->m10 + matrix1->m10;
        m_out->m11 = matrix0->m11 + matrix1->m11;
}

void add_matrix3x3 ( struct matrix3x3 *matrix0, struct matrix3x3 *matrix1, struct matrix3x3 *m_out )
{
        m_out->m00 = matrix0->m00 + matrix1->m00;
        m_out->m01 = matrix0->m01 + matrix1->m01;
        m_out->m02 = matrix0->m02 + matrix1->m02;

        m_out->m10 = matrix0->m10 + matrix1->m10;
        m_out->m11 = matrix0->m11 + matrix1->m11;
        m_out->m12 = matrix0->m12 + matrix1->m12;

        m_out->m20 = matrix0->m20 + matrix1->m20;
        m_out->m21 = matrix0->m21 + matrix1->m21;
        m_out->m22 = matrix0->m22 + matrix1->m22;
}

void add_matrix3x2 ( struct matrix3x2 *matrix0, struct matrix3x2 *matrix2, struct matrix3x2 *m_out )
{
        m_out->m00 = matrix0->m00 + matrix2->m00;
        m_out->m01 = matrix0->m01 + matrix2->m01;

        m_out->m10 = matrix0->m10 + matrix2->m10;
        m_out->m11 = matrix0->m11 + matrix2->m11;

        m_out->m20 = matrix0->m20 + matrix2->m20;
        m_out->m21 = matrix0->m21 + matrix2->m21;
}

void add_matrix4x4 ( struct matrix4x4 *matrix0, struct matrix4x4 *matrix1, struct matrix4x4 *m_out )
{
        m_out->m00 = matrix0->m00 + matrix1->m00;
        m_out->m01 = matrix0->m01 + matrix1->m01;
        m_out->m02 = matrix0->m02 + matrix1->m02;
        m_out->m03 = matrix0->m03 + matrix1->m03;

        m_out->m10 = matrix0->m10 + matrix1->m10;
        m_out->m11 = matrix0->m11 + matrix1->m11;
        m_out->m12 = matrix0->m12 + matrix1->m12;
        m_out->m13 = matrix0->m13 + matrix1->m13;

        m_out->m20 = matrix0->m20 + matrix1->m20;
        m_out->m21 = matrix0->m21 + matrix1->m21;
        m_out->m22 = matrix0->m22 + matrix1->m22;
        m_out->m23 = matrix0->m23 + matrix1->m23;

        m_out->m30 = matrix0->m30 + matrix1->m30;
        m_out->m31 = matrix0->m31 + matrix1->m31;
        m_out->m32 = matrix0->m32 + matrix1->m32;
        m_out->m33 = matrix0->m33 + matrix1->m33;
}

void add_matrix4x3 ( struct matrix4x3 *matrix0, struct matrix4x3 *matrix1, struct matrix4x3 *m_out )
{
        m_out->m00 = matrix0->m00 + matrix1->m00;
        m_out->m01 = matrix0->m01 + matrix1->m01;
        m_out->m02 = matrix0->m02 + matrix1->m02;

        m_out->m10 = matrix0->m10 + matrix1->m10;
        m_out->m11 = matrix0->m11 + matrix1->m11;
        m_out->m12 = matrix0->m12 + matrix1->m12;

        m_out->m20 = matrix0->m20 + matrix1->m20;
        m_out->m21 = matrix0->m21 + matrix1->m21;
        m_out->m22 = matrix0->m22 + matrix1->m22;

        m_out->m30 = matrix0->m30 + matrix1->m30;
        m_out->m31 = matrix0->m31 + matrix1->m31;
        m_out->m32 = matrix0->m32 + matrix1->m32;
}

void scale_matrix2x2 ( float k, struct matrix2x2 *matrix, struct matrix2x2 *m_out )
{
        m_out->m00 = k*matrix->m00;
        m_out->m01 = k*matrix->m01;

        m_out->m10 = k*matrix->m10;
        m_out->m11 = k*matrix->m11;
}

void scale_matrix3x3 ( float k, struct matrix3x3 *matrix, struct matrix3x3 *m_out )
{
        m_out->m00 = k*matrix->m00;
        m_out->m01 = k*matrix->m01;
        m_out->m02 = k*matrix->m02;

        m_out->m10 = k*matrix->m10;
        m_out->m11 = k*matrix->m11;
        m_out->m12 = k*matrix->m12;

        m_out->m20 = k*matrix->m20;
        m_out->m21 = k*matrix->m21;
        m_out->m22 = k*matrix->m22;
}

void scale_matrix3x2 ( float k, struct matrix3x2 *matrix, struct matrix3x2 *m_out )
{
        m_out->m00 = k*matrix->m00;
        m_out->m01 = k*matrix->m01;

        m_out->m10 = k*matrix->m10;
        m_out->m11 = k*matrix->m11;

        m_out->m20 = k*matrix->m20;
        m_out->m21 = k*matrix->m21;
}

void scale_matrix4x4 ( float k, struct matrix4x4 *matrix, struct matrix4x4 *m_out )
{
        m_out->m00 = k*matrix->m00;
        m_out->m01 = k*matrix->m01;
        m_out->m02 = k*matrix->m02;
        m_out->m03 = k*matrix->m03;

        m_out->m10 = k*matrix->m10;
        m_out->m11 = k*matrix->m11;
        m_out->m12 = k*matrix->m12;
        m_out->m13 = k*matrix->m13;

        m_out->m20 = k*matrix->m20;
        m_out->m21 = k*matrix->m21;
        m_out->m22 = k*matrix->m22;
        m_out->m23 = k*matrix->m23;

        m_out->m30 = k*matrix->m30;
        m_out->m31 = k*matrix->m31;
        m_out->m32 = k*matrix->m32;
        m_out->m33 = k*matrix->m33;
}

void scale_matrix4x3 ( float k, struct matrix4x3 *matrix, struct matrix4x3 *m_out )
{
        m_out->m00 = k*matrix->m00;
        m_out->m01 = k*matrix->m01;
        m_out->m02 = k*matrix->m02;

        m_out->m10 = k*matrix->m10;
        m_out->m11 = k*matrix->m11;
        m_out->m12 = k*matrix->m12;

        m_out->m20 = k*matrix->m20;
        m_out->m21 = k*matrix->m21;
        m_out->m22 = k*matrix->m22;

        m_out->m30 = k*matrix->m30;
        m_out->m31 = k*matrix->m31;
        m_out->m32 = k*matrix->m32;
}

/*
 * Multiplication of two matrices (ROW matrix)
 * |a00 a01 a02 a03|   |b00 b01 b02 b03|   |a00*b00 + a01*b10 + a02*b20 + a03*b30 ... ... ...|
 * |a10 a11 a12 a13| x |b10 b11 b12 b13| = |a10*b00 + a11*b10 + a12*b20 + a13*b30 ... ... ...|
 * |a20 a21 a22 a23|   |b20 b21 b22 b23|   |... a20*b01 + a21*b11 + a22*b21 + a23*b31 ... ...|
 * |a30 a31 a32 a33|   |b30 b31 b32 b33|   |... ... a30*b02 + a31*b12 + a32*b22 + a33*b32 ...|
 * Note that the result matrix MUST _NOT_ be the memory of any source matrices
 */
void mul_matrix2x2 ( struct matrix2x2 *matrix0, struct matrix2x2 *matrix1, struct matrix2x2 *m_out )
{
        // Row 0
        m_out->m00 = matrix0->m00*matrix1->m00 + matrix0->m01*matrix1->m10;
        m_out->m01 = matrix0->m00*matrix1->m01 + matrix0->m01*matrix1->m11;

        // Row 1
        m_out->m10 = matrix0->m10*matrix1->m00 + matrix0->m11*matrix1->m10;
        m_out->m11 = matrix0->m10*matrix1->m01 + matrix0->m11*matrix1->m11;
}

void mul_matrix3x3 ( struct matrix3x3 *matrix0, struct matrix3x3 *matrix1, struct matrix3x3 *m_out )
{
        // Row 0
        m_out->m00 = matrix0->m00*matrix1->m00 + matrix0->m01*matrix1->m10 + matrix0->m02*matrix1->m20;
        m_out->m01 = matrix0->m00*matrix1->m01 + matrix0->m01*matrix1->m11 + matrix0->m02*matrix1->m21;
        m_out->m02 = matrix0->m00*matrix1->m02 + matrix0->m01*matrix1->m12 + matrix0->m02*matrix1->m22;

        // Row 1
        m_out->m10 = matrix0->m10*matrix1->m00 + matrix0->m11*matrix1->m10 + matrix0->m12*matrix1->m20;
        m_out->m11 = matrix0->m10*matrix1->m01 + matrix0->m11*matrix1->m11 + matrix0->m12*matrix1->m21;
        m_out->m12 = matrix0->m10*matrix1->m02 + matrix0->m11*matrix1->m12 + matrix0->m12*matrix1->m22;

        // Row 2
        m_out->m20 = matrix0->m20*matrix1->m00 + matrix0->m21*matrix1->m10 + matrix0->m22*matrix1->m20;
        m_out->m21 = matrix0->m20*matrix1->m01 + matrix0->m21*matrix1->m11 + matrix0->m22*matrix1->m21;
        m_out->m22 = matrix0->m20*matrix1->m02 + matrix0->m21*matrix1->m12 + matrix0->m22*matrix1->m22;
}

void mul_matrix4x4 ( struct matrix4x4 *matrix0, struct matrix4x4 *matrix1, struct matrix4x4 *m_out )
{
        // Row 0
        m_out->m00 = matrix0->m00*matrix1->m00 + matrix0->m01*matrix1->m10 + matrix0->m02*matrix1->m20 +
                     matrix0->m03*matrix1->m30;
        m_out->m01 = matrix0->m00*matrix1->m01 + matrix0->m01*matrix1->m11 + matrix0->m02*matrix1->m21 +
                     matrix0->m03*matrix1->m31;
        m_out->m02 = matrix0->m00*matrix1->m02 + matrix0->m01*matrix1->m12 + matrix0->m02*matrix1->m22 +
                     matrix0->m03*matrix1->m32;
        m_out->m03 = matrix0->m00*matrix1->m03 + matrix0->m01*matrix1->m13 + matrix0->m02*matrix1->m23 +
                     matrix0->m03*matrix1->m33;

        // Row 1
        m_out->m10 = matrix0->m10*matrix1->m00 + matrix0->m11*matrix1->m10 + matrix0->m12*matrix1->m20 +
                     matrix0->m13*matrix1->m30;
        m_out->m11 = matrix0->m10*matrix1->m01 + matrix0->m11*matrix1->m11 + matrix0->m12*matrix1->m21 +
                     matrix0->m13*matrix1->m31;
        m_out->m12 = matrix0->m10*matrix1->m02 + matrix0->m11*matrix1->m12 + matrix0->m12*matrix1->m22 +
                     matrix0->m13*matrix1->m32;
        m_out->m13 = matrix0->m10*matrix1->m03 + matrix0->m11*matrix1->m13 + matrix0->m12*matrix1->m23 +
                     matrix0->m13*matrix1->m33;

        // Row 2
        m_out->m20 = matrix0->m20*matrix1->m00 + matrix0->m21*matrix1->m10 + matrix0->m22*matrix1->m20 +
                     matrix0->m23*matrix1->m30;
        m_out->m21 = matrix0->m20*matrix1->m01 + matrix0->m21*matrix1->m11 + matrix0->m22*matrix1->m21 +
                     matrix0->m23*matrix1->m31;
        m_out->m22 = matrix0->m20*matrix1->m02 + matrix0->m21*matrix1->m12 + matrix0->m22*matrix1->m22 +
                     matrix0->m23*matrix1->m32;
        m_out->m23 = matrix0->m20*matrix1->m03 + matrix0->m21*matrix1->m13 + matrix0->m22*matrix1->m23 +
                     matrix0->m23*matrix1->m33;

        // Row 3
        m_out->m30 = matrix0->m30*matrix1->m00 + matrix0->m31*matrix1->m10 + matrix0->m32*matrix1->m20 +
                     matrix0->m33*matrix1->m30;
        m_out->m31 = matrix0->m30*matrix1->m01 + matrix0->m31*matrix1->m11 + matrix0->m32*matrix1->m21 +
                     matrix0->m33*matrix1->m31;
        m_out->m32 = matrix0->m30*matrix1->m02 + matrix0->m31*matrix1->m12 + matrix0->m32*matrix1->m22 +
                     matrix0->m33*matrix1->m32;
        m_out->m33 = matrix0->m30*matrix1->m03 + matrix0->m31*matrix1->m13 + matrix0->m32*matrix1->m23 +
                     matrix0->m33*matrix1->m33;
}

/*
 * Multiplication of a point and a matrix (ROW matrix)
 *             |m00 m01 m02 m03|   (x*m00 + y*m10 + z*m20 + w*m30,
 * |x y z w| x |m10 m11 m12 m13| =  x*m01 + y*m11 + z*m21 + w*m31,
 *             |m20 m21 m22 m23|    x*m02 + y*m12 + z*m22 + w*m32,
 *             |m30 m31 m32 m33|    x*m03 + y*m13 + z*m23 + w*m33)
 * Note that the result point MUST _NOT_ be the source point
 */
void mul_point3d_matrix3x3 ( struct point3d *p, struct matrix3x3 *matrix, struct point3d *p_out )
{
        p_out->x = p->x*matrix->m00 + p->y*matrix->m10 + p->z*matrix->m20;
        p_out->y = p->x*matrix->m01 + p->y*matrix->m11 + p->z*matrix->m21;
        p_out->z = p->x*matrix->m02 + p->y*matrix->m12 + p->z*matrix->m22;
}

/*
 * This is mathematically incorrect, the row number of the first matrix
 * have to match the column number of the second matrix
 * but I just assume the w component of the point to be 1.0 just for convenience
 */
void mul_point3d_matrix4x4 ( struct point3d *p, struct matrix4x4 *matrix, struct point3d *p_out )
{
        p_out->x = p->x*matrix->m00 + p->y*matrix->m10 + p->z*matrix->m20 + 1.0f*matrix->m30;
        p_out->y = p->x*matrix->m01 + p->y*matrix->m11 + p->z*matrix->m21 + 1.0f*matrix->m31;
        p_out->z = p->x*matrix->m02 + p->y*matrix->m12 + p->z*matrix->m22 + 1.0f*matrix->m32;
}

void mul_point3d_matrix4x3 ( struct point3d *p, struct matrix4x3 *matrix, struct point3d *p_out )
{
        p_out->x = p->x*matrix->m00 + p->y*matrix->m10 + p->z*matrix->m20 + 1.0f*matrix->m30;
        p_out->y = p->x*matrix->m01 + p->y*matrix->m11 + p->z*matrix->m21 + 1.0f*matrix->m31;
        p_out->z = p->x*matrix->m02 + p->y*matrix->m12 + p->z*matrix->m22 + 1.0f*matrix->m32;
}

void mul_point4d_matrix4x4 ( struct point4d *p, struct matrix4x4 *matrix, struct point4d *p_out )
{
        p_out->x = p->x*matrix->m00 + p->y*matrix->m10 + p->z*matrix->m20 + p->w*matrix->m30;
        p_out->y = p->x*matrix->m01 + p->y*matrix->m11 + p->z*matrix->m21 + p->w*matrix->m31;
        p_out->z = p->x*matrix->m02 + p->y*matrix->m12 + p->z*matrix->m22 + p->w*matrix->m32;
        p_out->w = p->x*matrix->m03 + p->y*matrix->m13 + p->z*matrix->m23 + p->w*matrix->m33;
}

/*
 * This is mathematically incorrect, we lost the last component of the point
 * In this way, the function will have to perform linear transform only
 */
void mul_point4d_matrix4x3 ( struct point4d *p, struct matrix4x3 *matrix, struct point4d *p_out )
{
        p_out->x = p->x*matrix->m00 + p->y*matrix->m10 + p->z*matrix->m20 + p->w*matrix->m30;
        p_out->y = p->x*matrix->m01 + p->y*matrix->m11 + p->z*matrix->m21 + p->w*matrix->m31;
        p_out->z = p->x*matrix->m02 + p->y*matrix->m12 + p->z*matrix->m22 + p->w*matrix->m32;
        p_out->w = p->w;
}

/* Calculate the value determinant (use matrix as a determinant) */
float det_matrix2x2 ( struct matrix2x2 *matrix )
{
        return matrix->m00*matrix->m11 - matrix->m10*matrix->m01;
}

float det_matrix3x3 ( struct matrix3x3 *matrix )
{
        float product0, product1, product2;

        product0 = matrix->m11*matrix->m22 - matrix->m12*matrix->m21;
        product1 = matrix->m12*matrix->m20 - matrix->m10*matrix->m22;
        product2 = matrix->m10*matrix->m21 - matrix->m11*matrix->m20;

        return matrix->m00*product0 + matrix->m01*product1 + matrix->m02*product2;
}

float det_matrix4x4 ( struct matrix4x4 *matrix )
{
        float product0, product1, product2, product3;
        float subProduct0, subProduct1, subProduct2, subProduct3;

        subProduct0 = matrix->m22*matrix->m33 - matrix->m32*matrix->m23;
        subProduct1 = matrix->m21*matrix->m32 - matrix->m31*matrix->m22;
        subProduct2 = matrix->m23*matrix->m30 - matrix->m33*matrix->m20;
        subProduct3 = matrix->m20*matrix->m31 - matrix->m30*matrix->m21;

        product0 = matrix->m11*subProduct0 +
                   matrix->m12*(matrix->m23*matrix->m31 - matrix->m33*matrix->m21) +
                   matrix->m13*subProduct1;

        product1 = matrix->m12*subProduct2 +
                   matrix->m13*(matrix->m20*matrix->m32 - matrix->m30*matrix->m22) +
                   matrix->m10*subProduct0;

        product2 = matrix->m13*subProduct3 +
                   matrix->m10*(matrix->m21*matrix->m33 - matrix->m31*matrix->m23) +
                   matrix->m11*subProduct2;

        product3 = matrix->m10*subProduct1 +
                   matrix->m11*(matrix->m22*matrix->m30 - matrix->m32*matrix->m20) +
                   matrix->m12*subProduct3;

        return matrix->m00*product0 + matrix->m01*product1 +
               matrix->m02*product2 + matrix->m03*product3;
}

/*
 * Inverse of a matrix
 * M^(-1) = M* / det(M), in which M* is the adjoint matrix
 */
void inv_matrix2x2 ( struct matrix2x2 *matrix, struct matrix2x2 *m_out )
{
        float det = matrix->m00*matrix->m11 - matrix->m10*matrix->m01;
        float inv_det = 1.0f/det;

        m_out->m00 = inv_det*  matrix->m11;
        m_out->m01 = inv_det*(-matrix->m10);

        m_out->m10 = inv_det*(-matrix->m01);
        m_out->m11 = inv_det*  matrix->m11;
}

/*
 * This one is a little daunting
 * M* =
 *  |m11 m12|  |m01 m02|  |m01 m02|
 * +|m21 m22| -|m21 m22| +|m11 m12|
 *
 *  |m10 m12|  |m00 m02|  |m00 m02|
 * -|m20 m22| +|m20 m22| -|m10 m12|
 *
 *  |m10 m11|  |m00 m01|  |m00 m01|
 * +|m20 m21| -|m20 m21| +|m10 m11|
 */
void inv_matrix3x3 ( struct matrix3x3 *matrix, struct matrix3x3 *m_out )
{
        float product0 = matrix->m11*matrix->m22 - matrix->m12*matrix->m21;
        float product1 = matrix->m12*matrix->m20 - matrix->m10*matrix->m22;
        float product2 = matrix->m10*matrix->m21 - matrix->m11*matrix->m20;

        float det = matrix->m00*product0 + matrix->m01*product1 + matrix->m02*product2;
        float inv_det = 1.0f/det;

        m_out->m00 =  inv_det*product0;
        m_out->m01 = -inv_det*(matrix->m01*matrix->m22 - matrix->m21*matrix->m02);
        m_out->m02 =  inv_det*(matrix->m01*matrix->m12 - matrix->m11*matrix->m02);

        m_out->m10 = -inv_det*(-product1);
        m_out->m11 =  inv_det*(matrix->m00*matrix->m22 - matrix->m20*matrix->m02);
        m_out->m12 = -inv_det*(matrix->m00*matrix->m12 - matrix->m10*matrix->m02);

        m_out->m20 =  inv_det*product2;
        m_out->m21 = -inv_det*(matrix->m00*matrix->m21 - matrix->m20*matrix->m01);
        m_out->m22 =  inv_det*(matrix->m00*matrix->m11 - matrix->m10*matrix->m01);
}

#define det_m3( m00, m01, m02,\
	        m10, m11, m12,\
	        m20, m21, m22, _sign, _det)	\
{\
	float _product0 = m11*m22 - m12*m21;\
	float _product1 = m12*m20 - m10*m22;\
	float _product2 = m10*m21 - m11*m20;\
	_det = _sign*(m00*_product0 + m01*_product1 + m02*_product2);\
}

void inv_matrix4x4 ( struct matrix4x4 *matrix, struct matrix4x4 *m_out )
{
        float inv_det = 1.0f/det_matrix4x4 ( matrix );
        det_m3 ( matrix->m11, matrix->m12, matrix->m13,
                 matrix->m21, matrix->m22, matrix->m23,
                 matrix->m31, matrix->m32, matrix->m33, inv_det, m_out->m00 );
        det_m3 ( matrix->m01, matrix->m02, matrix->m03,
                 matrix->m21, matrix->m22, matrix->m23,
                 matrix->m31, matrix->m32, matrix->m33, -inv_det, m_out->m00 );
        det_m3 ( matrix->m01, matrix->m02, matrix->m03,
                 matrix->m11, matrix->m12, matrix->m13,
                 matrix->m31, matrix->m32, matrix->m33, inv_det, m_out->m00 );
        det_m3 ( matrix->m01, matrix->m02, matrix->m03,
                 matrix->m11, matrix->m12, matrix->m13,
                 matrix->m21, matrix->m22, matrix->m23, -inv_det, m_out->m00 );

        det_m3 ( matrix->m10, matrix->m12, matrix->m13,
                 matrix->m20, matrix->m22, matrix->m23,
                 matrix->m30, matrix->m32, matrix->m33, inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m02, matrix->m03,
                 matrix->m20, matrix->m22, matrix->m23,
                 matrix->m30, matrix->m32, matrix->m33, -inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m02, matrix->m03,
                 matrix->m10, matrix->m12, matrix->m13,
                 matrix->m30, matrix->m32, matrix->m33, inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m02, matrix->m03,
                 matrix->m10, matrix->m12, matrix->m13,
                 matrix->m20, matrix->m22, matrix->m23, -inv_det, m_out->m00 );

        det_m3 ( matrix->m10, matrix->m11, matrix->m13,
                 matrix->m20, matrix->m21, matrix->m23,
                 matrix->m30, matrix->m31, matrix->m33, inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m01, matrix->m03,
                 matrix->m20, matrix->m21, matrix->m23,
                 matrix->m30, matrix->m31, matrix->m33, -inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m01, matrix->m03,
                 matrix->m10, matrix->m11, matrix->m13,
                 matrix->m30, matrix->m31, matrix->m33, inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m01, matrix->m03,
                 matrix->m10, matrix->m11, matrix->m13,
                 matrix->m20, matrix->m21, matrix->m23, -inv_det, m_out->m00 );

        det_m3 ( matrix->m10, matrix->m11, matrix->m12,
                 matrix->m20, matrix->m21, matrix->m22,
                 matrix->m30, matrix->m31, matrix->m32, inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m01, matrix->m02,
                 matrix->m20, matrix->m21, matrix->m22,
                 matrix->m30, matrix->m31, matrix->m32, -inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m01, matrix->m02,
                 matrix->m10, matrix->m11, matrix->m12,
                 matrix->m30, matrix->m31, matrix->m32, inv_det, m_out->m00 );
        det_m3 ( matrix->m00, matrix->m01, matrix->m02,
                 matrix->m10, matrix->m11, matrix->m12,
                 matrix->m20, matrix->m21, matrix->m22, -inv_det, m_out->m00 );
}

#undef det_m3

/* Solve a linear system, with the result tested */
int solve_2x2_cramer_s ( struct matrix2x2 *coe, struct vector2d *c, float *x, float *y )
{
        float det = det_matrix2x2 ( coe );
        if ( fabsf ( det ) < DEFAULT_PRECISION ) {
                return 0;
        }
        float inv_det = 1.0f/det;
        *x = (c->x*coe->m11 - c->y*coe->m01)*inv_det;
        *y = (coe->m00*c->y - coe->m10*c->x)*inv_det;
        return 1;
}

void solve_2x2_cramer ( struct matrix2x2 *coe, struct vector2d *c, float *x, float *y )
{
        float inv_det = 1.0f/det_matrix2x2 ( coe );
        *x = (c->x*coe->m11 - c->y*coe->m01)*inv_det;
        *y = (coe->m00*c->y - coe->m10*c->x)*inv_det;
}

int solve_3x3_cramer_s ( struct matrix3x3 *coe, struct vector3d *c, float *x, float *y, float *z )
{
        float det = det_matrix3x3 ( coe );

        if ( fabsf ( det ) < DEFAULT_PRECISION ) {
                return 0;
        }
        float inv_det = 1.0f/det;
        *x = (c->x*    (coe->m11*coe->m22 - coe->m21*coe->m12) +
              coe->m01*(coe->m12*c->z - coe->m22*c->y) +
              coe->m02*(c->y*coe->m21 - c->z*coe->m11))*inv_det;

        *y = (coe->m00*(c->y*coe->m22 - c->z*coe->m12) +
              c->x*    (coe->m12*coe->m20 - coe->m22*coe->m10) +
              coe->m02*(coe->m10*c->z - coe->m20*c->y))*inv_det;

        *z = (coe->m00*(coe->m11*c->z - coe->m21*c->y) +
              coe->m01*(c->y*coe->m20 - c->z*coe->m10) +
              c->x*    (coe->m10*coe->m21 - coe->m20*coe->m11))*inv_det;
        return 1;
}

void solve_3x3_cramer ( struct matrix3x3 *coe, struct vector3d *c, float *x, float *y, float *z )
{
        float inv_det = 1.0f/det_matrix3x3 ( coe );
        *x = (c->x*    (coe->m11*coe->m22 - coe->m21*coe->m12) +
              coe->m01*(coe->m12*c->z - coe->m22*c->y) +
              coe->m02*(c->y*coe->m21 - c->z*coe->m11))*inv_det;

        *y = (coe->m00*(c->y*coe->m22 - c->z*coe->m12) +
              c->x*    (coe->m12*coe->m20 - coe->m22*coe->m10) +
              coe->m02*(coe->m10*c->z - coe->m20*c->y))*inv_det;

        *z = (coe->m00*(coe->m11*c->z - coe->m21*c->y) +
              coe->m01*(c->y*coe->m20 - c->z*coe->m10) +
              c->x*    (coe->m10*coe->m21 - coe->m20*coe->m11))*inv_det;
}

/* Some transform matrices */
void get_translate_matrix4x4 ( float x, float y, float z, struct matrix4x4 *m_out )
{
        m_out->m00 = 1.0f;
        m_out->m01 = 0.0f;
        m_out->m02 = 0.0f;
        m_out->m03 = 0.0f;

        m_out->m10 = 0.0f;
        m_out->m11 = 1.0f;
        m_out->m12 = 0.0f;
        m_out->m13 = 0.0f;

        m_out->m20 = 0.0f;
        m_out->m21 = 0.0f;
        m_out->m22 = 1.0f;
        m_out->m23 = 0.0f;

        m_out->m30 = x;
        m_out->m31 = y;
        m_out->m32 = z;
        m_out->m33 = 1.0f;
}

void get_rotate_x_matrix4x4 ( float euler_x, struct matrix4x4 *m_out )
{
        float sin = sinf ( deg_to_rad ( euler_x ) );
        float cos = cosf ( deg_to_rad ( euler_x ) );

        m_out->m00 = 1.0f;
        m_out->m01 = 0.0f;
        m_out->m02 = 0.0f;
        m_out->m03 = 0.0f;

        m_out->m10 = 0.0f;
        m_out->m11 = cos;
        m_out->m12 = -sin;
        m_out->m13 = 0.0f;

        m_out->m20 = 0.0f;
        m_out->m21 = sin;
        m_out->m22 = cos;
        m_out->m23 = 0.0f;
        m_out->m30 = 0.0f;

        m_out->m31 = 0.0f;
        m_out->m32 = 0.0f;
        m_out->m33 = 1.0f;
}

void get_rotate_y_matrix4x4 ( float euler_y, struct matrix4x4 *m_out )
{
        float sin = sinf ( deg_to_rad ( euler_y ) );
        float cos = cosf ( deg_to_rad ( euler_y ) );

        m_out->m00 = cos;
        m_out->m01 = 0.0f;
        m_out->m02 = -sin;
        m_out->m03 = 0.0f;

        m_out->m10 = 0.0f;
        m_out->m11 = 1.0f;
        m_out->m12 = 0.0f;
        m_out->m13 = 0.0f;

        m_out->m20 = sin;
        m_out->m21 = 0.0f;
        m_out->m22 = cos;
        m_out->m23 = 0.0f;

        m_out->m30 = 0.0f;
        m_out->m31 = 0.0f;
        m_out->m32 = 0.0f;
        m_out->m33 = 1.0f;
}

void get_rotate_z_matrix4x4 ( float euler_z, struct matrix4x4 *m_out )
{
        float sin = sinf ( deg_to_rad ( euler_z ) );
        float cos = cosf ( deg_to_rad ( euler_z ) );

        m_out->m00 = -sin;
        m_out->m01 = cos;
        m_out->m02 = 0.0f;
        m_out->m03 = 0.0f;

        m_out->m10 = cos;
        m_out->m11 = sin;
        m_out->m12 = 0.0f;
        m_out->m13 = 0.0f;

        m_out->m20 = 0.0f;
        m_out->m21 = 0.0f;
        m_out->m22 = 1.0f;
        m_out->m23 = 0.0f;

        m_out->m30 = 0.0f;
        m_out->m31 = 0.0f;
        m_out->m32 = 0.0f;
        m_out->m33 = 1.0f;
}

void get_rotate_xyz_matrix4x4 ( float euler_x, float euler_y, float euler_z, struct matrix4x4 *m_out )
{
        struct matrix4x4 tmp;
        get_rotate_z_matrix4x4 ( euler_z, m_out );
        get_rotate_y_matrix4x4 ( euler_y, &tmp );
        mul_matrix4x4_u ( m_out, &tmp );
        get_rotate_x_matrix4x4 ( euler_x, &tmp );
        mul_matrix4x4_u ( m_out, &tmp );
}

/*
 * For transformation that transform current coordinate system
 * to a subsystem defined by a set of basis vectors
 */
void get_rotate_to_uvn_matrix4x4 ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
                                   struct matrix4x4 *m_out )
{
        m_out->m00 = u->x;
        m_out->m01 = v->x;
        m_out->m02 = n->x;
        m_out->m03 = 0.0f;

        m_out->m10 = u->y;
        m_out->m11 = v->y;
        m_out->m12 = n->y;
        m_out->m13 = 0.0f;

        m_out->m20 = u->z;
        m_out->m21 = v->z;
        m_out->m22 = n->z;
        m_out->m23 = 0.0f;

        m_out->m30 = 0.0f;
        m_out->m31 = 0.0f;
        m_out->m32 = 0.0f;
        m_out->m33 = 1.0f;
}

/* scaling factor [0.0f, 1.0f] for each x, y, z */
void get_scale_matrix4x4 ( float x, float y, float z, struct matrix4x4 *m_out )
{
        m_out->m00 = x;
        m_out->m01 = 0.0f;
        m_out->m02 = 0.0f;
        m_out->m03 = 0.0f;

        m_out->m10 = 0.0f;
        m_out->m11 = y;
        m_out->m12 = 0.0f;
        m_out->m13 = 0.0f;

        m_out->m20 = 0.0f;
        m_out->m21 = 0.0f;
        m_out->m22 = z;
        m_out->m23 = 0.0f;

        m_out->m30 = 0.0f;
        m_out->m31 = 0.0f;
        m_out->m32 = 0.0f;
        m_out->m33 = 1.0f;
}
