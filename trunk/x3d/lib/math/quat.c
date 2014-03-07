/* quad.c: All Quaternion maths functions go here */
#include "vector.h"
#include "quat.h"



void clear_quad ( struct quat *q )
{
        q->x = 0.0f;
        q->y = 0.0f;
        q->z = 0.0f;
        q->w = 0.0f;
}

void init_quat ( float w, float x, float y, float z, struct quat *q )
{
        q->w = w;
        q->x = x;
        q->y = y;
        q->z = z;
}

void init_quat_vector3d ( struct quat *v, struct quat *q )
{
        q->w = 0.0f;
        q->x = v->x;
        q->y = v->y;
        q->z = v->z;
}

void copy_quat ( struct quat *q0, struct quat *q1 )
{
        q1->w = q0->w;
        q1->x = q0->x;
        q1->y = q0->y;
        q1->z = q0->z;
}

void add_quat ( struct quat *q0, struct quat *q1, struct quat *qr )
{
        qr->w = q0->w + q1->w;
        qr->x = q0->x + q1->x;
        qr->y = q0->y + q1->y;
        qr->z = q0->z + q1->z;
}

void sub_quat ( struct quat *q0, struct quat *q1, struct quat *qr )
{
        qr->w = q0->w - q1->w;
        qr->x = q0->x - q1->x;
        qr->y = q0->y - q1->y;
        qr->z = q0->z - q1->z;
}

void mul_quat ( struct quat *q0, struct quat *q1, struct quat *qr )
{
        qr->w = q0->w*q1->w - q0->x*q1->x - q0->y*q1->y - q0->z*q1->z;
        qr->x = q0->w*q1->x + q0->x*q1->w + q0->y*q1->z - q0->z*q1->y;
        qr->y = q0->w*q1->y - q0->x*q1->z + q0->y*q1->w + q0->z*q1->x;
        qr->z = q0->w*q1->z + q0->x*q1->y - q0->y*q1->x + q0->z*q1->w;
}

void eul_angle_to_quat ( float x, float y, float z, struct quat *qr )
{
}

void rotate_point3d_quat ( struct point3d *p0, struct quat *qr, struct point3d *pr )
{
}
