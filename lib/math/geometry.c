// gemetry.c: All functions related to shapes, planes ,lines/ray and box go here
#include "math.h"
#include "vector.h"
#include "matrix.h"
#include "geometry.h"



void build_line2d ( struct point2d *p0, struct point2d *p1, struct line2d *l )
{
        copy_point2d ( p0, &l->p0 );
        copy_point2d ( p1, &l->p1 );
        build_vector2d ( p0, p1, &l->v );
        float len = length_vector2d ( &l->v );
        float inv = 1.0f/len;
        l->v.x *= inv;
        l->v.y *= inv;
        l->t0 = 0.0f;
        l->t1 = len;
}

void build_line2d_t ( struct point2d *p0, struct point2d *p1, float t0, float t1, struct line2d *l )
{
        copy_point2d ( p0, &l->p0 );
        copy_point2d ( p1, &l->p1 );
        build_vector2d ( p0, p1, &l->v );
        float len = length_vector2d ( &l->v );
        float inv = 1.0f/len;
        l->v.x *= inv;
        l->v.y *= inv;
        float j0 = 0.0f;
        float j1 = len;
        int_interval ( t0, t1, j0, j1, l->t0, l->t1 );
}

void build_line3d ( struct point3d *p0, struct point3d *p1, struct line3d *l )
{
        copy_point3d ( p0, &l->p0 );
//        copy_point3d ( p1, &l->p1 );
        build_vector3d ( p0, p1, &l->v );
        float len = length_vector3d ( &l->v );
        float inv = 1.0f/len;
        l->v.x *= inv;
        l->v.y *= inv;
        l->v.z *= inv;
        l->t0 = 0.0f;
        l->t1 = len;
}

void build_line3d_t ( struct point3d *p0, struct point3d *p1, float t0, float t1, struct line3d *l )
{
        copy_point3d ( p0, &l->p0 );
//        copy_point3d ( p1, &l->p1 );
        build_vector3d ( p0, p1, &l->v );
        float len = length_vector3d ( &l->v );
        float inv = 1.0f/len;
        l->v.x *= inv;
        l->v.y *= inv;
        l->v.z *= inv;
        float j0 = 0.0f;
        float j1 = len;
        l->t0 = max(j0, t0);
        l->t1 = max(j1, t1);
}

void ray3d_build_t(struct ray3d *self, struct point3d *p0, struct point3d *p1, float t0)
{
        self->p0 = *p0;
        vector3d_comps(self->v.v[i] = p1->p[i] - p0->p[i]);
        self->len = length_vector3d(&self->v);
        float inv = 1.0f/self->len;
        self->v.x *= inv;
        self->v.y *= inv;
        self->v.z *= inv;
        self->inv_len2 = inv*inv;
        self->t0 = max(t0, EPSILON_E4);
        self->t1 = self->len;
}
//
//void ray3d_build_t2(struct ray3d *self, struct point3d *p0, struct point3d *p1, float t0, float t1)
//{
//        self->p0 = *p0;
//        vector3d_comps(self->v.v[i] = p1->p[i] - p0->p[i]);
//        self->len = 1.0f;
//        self->inv_len2 = 1.0f;
//        self->t0 = max(t0, EPSILON_E4);
//        self->t1 = t1;
//}

void ray3d_build_t3(struct ray3d *self, struct point3d *p0, struct vector3d* v, float t0, float t1)
{
        self->p0 = *p0;
        self->v = *v;
        self->len = 1.0f;
        self->inv_len2 = 1.0f;
        self->t0 = max(t0, EPSILON_E4);
        self->t1 = t1;
}

float ray3d_length(struct ray3d* self)
{
        return self->len;
}

float ray3d_inv_length2(struct ray3d* self)
{
        return self->inv_len2;
}

void ray3d_direction(struct ray3d* self, struct vector3d* v)
{
        *v = self->v;
}

void build_plane3d ( struct vector3d *n, struct point3d *p, struct plane3d *plane )
{
        copy_vector3d ( n, &plane->n );
        copy_point3d ( p, &plane->p0 );
}

/* Normalized version */
void build_plane3d_n ( struct vector3d *n, struct point3d *p, struct plane3d *plane )
{
        copy_vector3d ( n, &plane->n );
        normalize_vector3d_u ( &plane->n );
        copy_point3d ( p, &plane->p0 );
}

void build_rectangle_2d ( float x0, float x1, float y0, float y1, struct rectangle2d *rect )
{
        rect->x0 = x0;
        rect->x1 = x1;
        rect->y0 = y0;
        rect->y1 = y1;
}

void build_irectangle_2d ( int x0, int x1, int y0, int y1, struct irectangle2d *rect )
{
        rect->x0 = x0;
        rect->x1 = x1;
        rect->y0 = y0;
        rect->y1 = y1;
}

void point_on_line2d ( struct line2d *l, float t, struct point2d *p )
{
        sscale_add_vector2d ( &l->p0, t, &l->v, p );
}

void point_on_line3d ( struct line3d *l, float t, struct point3d *p )
{
        sscale_add_vector3d ( &l->p0, t, &l->v, p );
}

int intersect_line2d ( struct line2d *l0, struct line2d *l1, float *t0_out, float *t1_out )
{
        struct matrix2x2 coe;
        set_matrix2x2 ( &coe,
                        l0->v.x, l1->v.x,
                        l0->v.y, l1->v.y );
        struct vector2d c;
        sub_vector2d ( &l0->p0, &l1->p0, &c );
        float t0, t1;
        solve_2x2_cramer ( &coe, &c, &t0, &t1 );
        if ( in_interval ( t0, l0->t0, l0->t1 ) &&
             in_interval ( t1, l1->t0, l1->t1 ) ) {
                return IN_SEGMENT;
        } else {
                return OUT_SEGMENT;
        }
}

/*
 * Justify if the point is on the plane, the positive half, or the negative half by
 * knowing the dot product between plane normal and the vector from p0 to the given point
 */
float in_plane3d ( struct point3d *p, struct plane3d *plane )
{
        struct vector3d v;
        build_vector3d ( &plane->p0, p, &v );
        return dot_vector3d ( &plane->n, &v );;
}

/*
 * Find line-plane intersection
 * Nx(Px + Vx*t - x0) + Ny(Py + Vy*t - y0) + Nz(Pz + Vz*t - z0) = 0
 */
int intersect_line_plane3d ( struct plane3d *pl, struct line3d *l, float *t, struct point3d *p )
{
        float tr = -(pl->n.x*l->p0.x + pl->n.y*l->p0.y + pl->n.z*l->p0.z -
                     pl->n.x*l->p0.x - pl->n.y*pl->p0.y - pl->n.z*pl->p0.z)/
                   (l->v.x*pl->n.x + l->v.y*pl->n.y + l->v.z*pl->n.z);
        if ( !in_interval ( tr, l->t0, l->t1 ) ) {
                return OUT_SEGMENT;
        }
        point_on_line3d ( l, tr, p );
        *t = tr;
        return IN_SEGMENT;
}

/* Rotate a point in 2d around origin with specified angle */
void rotate_o_point2d ( struct point2d *p, float theta, struct point2d *p_out )
{
        float sin_the, cos_the;
        cos_the = cosf ( deg_to_rad ( theta ) );
        sin_the = sinf ( deg_to_rad ( theta ) );
        p_out->x = p->x*cos_the - p->y*sin_the;
        p_out->y = p->x*sin_the + p->y*cos_the;
}

/* around an arbitary point */
void rotate_a_point2d ( struct point2d *p, struct point2d *o, float theta, struct point2d *p_out )
{
        float sin_the, cos_the;
        cos_the = cosf ( deg_to_rad ( theta ) );
        sin_the = sinf ( deg_to_rad ( theta ) );
        struct point2d t;
        sub_point2d ( p, o, &t );
        p_out->x = t.x*cos_the - t.y*sin_the;
        p_out->y = t.x*sin_the + t.y*cos_the;
        add_point2d_u ( p_out, &t );
}

/* Rotate a point in 3d around x axis */
void rotate_x_point3d ( struct point3d *p, float theta, struct point3d *p_out )
{
        float sin_the, cos_the;
        cos_the = cosf ( deg_to_rad ( theta ) );
        sin_the = sinf ( deg_to_rad ( theta ) );
        /* Multiply a transforming matrix virtually ( No changing of X ) */
        p_out->x =  p->x;
        p_out->y =  p->y*cos_the + p->z*sin_the;
        p_out->z = -p->y*sin_the + p->z*cos_the;
}

/* Rotate a point in 3d around y axis */
void rotate_y_point3d ( struct point3d *p, float theta, struct point3d *p_out )
{
        float sin_the, cos_the;
        cos_the = cosf ( deg_to_rad ( theta ) );
        sin_the = sinf ( deg_to_rad ( theta ) );
        /* Multiply a transforming matrix virtually ( No changing of Y ) */
        p_out->x =  p->x*cos_the + p->z*sin_the;
        p_out->y =  p->y;
        p_out->z = -p->x*sin_the + p->z*cos_the;
}

/* Rotate a point in 3d around z axis */
void rotate_z_point3d ( struct point3d *p, float theta, struct point3d *p_out )
{
        float sin_the, cos_the;
        cos_the = cosf ( deg_to_rad ( theta ) );
        sin_the = sinf ( deg_to_rad ( theta ) );
        /* Multiply a transforming matrix virtually ( No changing of Z ) */
        p_out->x = -p->x*sin_the + p->y*cos_the;
        p_out->y =  p->x*cos_the + p->y*sin_the;
        p_out->z =  p->z;
}

/* Rotate a point in 3d around an arbitary axis */
void rotate_a_point3d ( struct vector3d *a, struct point3d *p, float theta, struct point3d *p_out )
{
        normalize_vector3d_u ( a );

        /* Create a new virtual base */
        /* v2 = axis*(axis.p) */
        /* v0 = p0 - v2 */
        /* v1 = v0 x v2 */
        struct vector3d v0, v1, v2;
        scale_vector3d ( dot_vector3d ( a, p ), a, &v2 );
        sub_vector3d ( p, &v2, &v0 );
        cross_vector3d ( &v0, &v2, &v1 );

        /* Rotate v0 around the "axis" */
        float sin_the = sinf ( deg_to_rad ( theta ) );
        float cos_the = cosf ( deg_to_rad ( theta ) );
        struct vector3d vr;
        dscale_add_vector3d ( cos_the, &v0, sin_the, &v1, &vr );

        /* Translate it back to the normal coordinate system */
        add_point3d ( &vr, &v2, p_out );
}

void build_sphere3d ( float r, float z0, float z1, struct sphere3d *sp )
{
        sp->r = r;
        sp->z0 = z0;
        sp->z1 = z1;
}

void build_cylinder3d ( float r, float z0, float z1, struct cylinder3d *cy )
{
        cy->r = r;
        cy->z0 = z0;
        cy->z1 = z1;
}

void build_disk3d ( float ir, float r, float h, struct disk3d *d )
{
        d->ir = ir;
        d->r = r;
        d->h = h;
}

void build_cone3d ( float r, float h, struct cone3d *c )
{
        c->a = fsqr (h)/fsqr (r);
        c->r = r;
        c->h = h;
}

void build_paraboloid3d ( float r, float h0, float h1, struct paraboloid3d *pa )
{
        pa->a = h1/fsqr (r);
        pa->r = r;
        pa->h0 = h0;
        pa->h1 = h1;
}

void build_hyperboloid3d ( struct point3d *p0, struct point3d *p1, float z0, float z1,
                           struct hyperboloid3d *hy )
{
        copy_point3d ( p0, &hy->p0 );
        copy_point3d ( p1, &hy->p1 );
        float d = fsqr( p1->z )*(fsqr( p0->x ) + fsqr( p0->y )) -
                  fsqr( p0->z )*(fsqr( p1->x ) + fsqr( p1->y ));
        float a2 = d/(fsqr( p1->z ) - fsqr( p0->z ));
        float c2 = d/((fsqr ( p1->x ) + fsqr ( p1->y )) - (fsqr ( p0->x ) + fsqr ( p0->y )));
        hy->a2 = 1.0f/a2;
        hy->c2 = 1.0f/c2;
        hy->z0 = z0;
        hy->z1 = z1;
        float r0 = a2*(1.0f + fsqr (z0)/c2);
        float r1 = a2*(1.0f + fsqr (z1)/c2);
        r0 = max ( r0, r1 );
        hy->r = sqrtf ( r0 );
}

void build_triangle3d ( struct point3d *p0, struct point3d *p1, struct point3d *p2, struct triangle3d *tri )
{
        copy_point3d ( p0, &tri->v[0] );
        copy_point3d ( p1, &tri->v[1] );
        copy_point3d ( p2, &tri->v[2] );
}

void triangle3d_normal(struct triangle3d* self, struct vector3d* n)
{
        struct vector3d u, v;
        build_vector3d(&self->v[0], &self->v[1], &u);
        build_vector3d(&self->v[0], &self->v[2], &v);
        cross_vector3d(&u, &v, n);
        normalize_vector3d_u(n);
}

void build_triangle4d ( struct point3d *p0, struct point3d *p1, struct point3d *p2, struct triangle4d *tri )
{
        set_point4d ( p0->x, p0->y, p0->z, 1.0f, &tri->v[0] );
        set_point4d ( p0->x, p0->y, p0->z, 1.0f, &tri->v[1] );
        set_point4d ( p0->x, p0->y, p0->z, 1.0f, &tri->v[2] );
}

void build_triangle4d_w ( struct point4d *p0, struct point4d *p1, struct point4d *p2, struct triangle4d *tri )
{
        copy_point4d ( p0, &tri->v[0] );
        copy_point4d ( p1, &tri->v[1] );
        copy_point4d ( p2, &tri->v[2] );
}

/* Find line-sphere intersection and compute bias of the intersect
 * Note that the computation is performed at local coordinate system
 * f(x, y, z) = x^2 + y^2 + z^2 - r^2 = 0 */
int intersect_line_sphere3d ( struct line3d *l, struct sphere3d *sp,
                              float *t, struct point3d *p, float *bias )
{
        /* (Px + Vx*t)^2 + (Py + Vy*t)^2 + (Pz + Vz*t)^2 - r = 0 */
        float a = fsqr ( l->v.x ) + fsqr ( l->v.y ) + fsqr ( l->v.z );
        float b = 2.0f*(l->v.x*l->p0.x + l->v.y*l->p0.y + l->v.z*l->p0.z);
        float c = fsqr ( l->p0.x ) + fsqr ( l->p0.y ) + fsqr ( l->p0.z ) - sp->r;
        float t0, t1;
        if ( !solve_quadratic ( a, b, c, &t0, &t1 ) ) {
                return OUT_SEGMENT;
        }

        if ( t1 < t0 ) {
                swap ( t0, t1 );
        }
        /* Test if both solution are out of bound ? if not, take one valid */
        if ( t0 > l->t1 || t1 < l->t0 ) {
                return OUT_SEGMENT;
        } else {
                if ( t0 < l->t0 ) {
                        t0 = t1;
                } else {
                        t0 = t0;
                }
        }

        point_on_line3d ( l, t0, p );

        /* Out of the boundary of z-axis */
        if ( !in_interval ( p->z, sp->z0, sp->z1 ) ) {
                return OUT_SEGMENT;
        }
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* Find line-cylinder intersection and compute bias of the intersect
 * Note that the computation is performed at local coordinate system
 * f(x, y) = x^2 + x^y - r^2 = 0 */
int intersect_line_cylinder3d ( struct line3d *l, struct cylinder3d *cy,
                                float *t, struct point3d *p, float *bias )
{
        /* (Px + Vx*t)^2 + (Py + Vy*t)^2 - r^2 = 0 */
        float a = fsqr ( l->v.x ) + fsqr ( l->v.y );
        float b = 2.0f*(l->v.x*l->p0.x + l->v.y*l->p0.y);
        float c = fsqr ( l->p0.x ) + fsqr ( l->p0.y ) - fsqr ( cy->r );

        float t0, t1;
        if ( !solve_quadratic ( a, b, c, &t0, &t1 ) ) {
                return OUT_SEGMENT;
        }

        if ( t1 < t0 ) {
                swap ( t0, t1 );
        }

        /* Test if both solution are out of bound ? if not, take one valid */
        if ( t0 > l->t1 || t1 < l->t0 ) {
                return OUT_SEGMENT;
        } else {
                if ( t0 < l->t0 ) {
                        t0 = t1;
                } else {
                        t0 = t0;
                }
        }

        point_on_line3d ( l, t0, p );

        /* Out of the boundary of z-axis */
        if ( !in_interval ( p->z, cy->z0, cy->z1 ) ) {
                return OUT_SEGMENT;
        }
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* Find line-disk intersection and compute bias of the intersect
 * Note that the computation is performed at local coordinate system
 * f(x, y, z) = h */
int intersect_line_disk3d ( struct line3d *l, struct disk3d *d,
                            float *t, struct point3d *p, float *bias )
{
        /* Pz + Vz*t = h */
        float t0 = (d->h - l->p0.z)/l->v.z;

        if ( !in_interval ( t0, l->t0, l->t1 ) ) {
                return OUT_SEGMENT;
        }
        point_on_line3d ( l, t0, p );

        /* Test if it is inside the range of [ir, r] */
        float dist2 = fsqr ( p->x ) + fsqr ( p->y );
        if ( !in_interval ( dist2, fsqr(d->ir), fsqr(d->r) ) ) {
                return OUT_SEGMENT;
        }
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* Find line-cone intersection and compute bias of the intersect
 * Note that the computation is performed at local coordinate system
 * f(x, y, z) = (hx)^2/r^2 + (hy)^2/r^2 - (z - h) = 0 */
int intersect_line_cone3d ( struct line3d *l, struct cone3d *cn,
                            float *t, struct point3d *p, float *bias )
{
        /* (h/r)^2[(Px + Vx*t)^2 + (Py + Vy*t)^2] - (Pz + Vz*t - h)^2 = 0 */
        float a = cn->a*(fsqr (l->v.x) + fsqr (l->v.y)) - fsqr (l->v.z);
        float b = 2.0f*(cn->a*(l->v.x*l->p0.x + l->v.y*l->p0.y) + l->v.z*(cn->h - l->p0.z));
        float c = cn->a*(fsqr (l->p0.x) + fsqr (l->p0.y)) - fsqr (l->p0.z - cn->h);

        float t0, t1;
        if ( !solve_quadratic ( a, b, c, &t0, &t1 ) ) {
                return OUT_SEGMENT;
        }

        if ( t1 < t0 ) {
                swap ( t0, t1 );
        }
        /* Test if both solution are out of bound ? if not, take one valid */
        if ( t0 > l->t1 || t1 < l->t0 ) {
                return OUT_SEGMENT;
        } else {
                if ( t0 < l->t0 ) {
                        t0 = t1;
                } else {
                        t0 = t0;
                }
        }

        point_on_line3d ( l, t0, p );

        /* Out of the boundary of height [0, h] */
        if ( !in_interval ( p->z, 0.0f, cn->h ) ) {
                return OUT_SEGMENT;
        }
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* Find line-paraboloid intersection and compute bias of the intersect
 * Note that the computation is performed at local coordinate system
 * f(x, y, z) = h/r^2*(x^2 + y^2) - z = 0 */
int intersect_line_paraboloid3d ( struct line3d *l, struct paraboloid3d *pa,
                                  float *t, struct point3d *p, float *bias )
{
        /* h/r^2[(Px + Vx*t)^2 + (Py + Vy*t)^2] - (Pz + Vz*t) = 0 */
        float a = pa->a*(fsqr (l->v.x) + fsqr (l->v.y));
        float b = 2.0f*pa->a*(l->v.x*l->p0.x + l->v.y*l->p0.y) - l->v.z;
        float c = pa->a*(fsqr (l->p0.x) + fsqr (l->p0.y)) - l->p0.z;

        float t0, t1;
        if ( !solve_quadratic ( a, b, c, &t0, &t1 ) ) {
                return OUT_SEGMENT;
        }

        if ( t1 < t0 ) {
                swap ( t0, t1 );
        }
        /* Test if both solution are out of bound ? if not, take one valid */
        if ( t0 > l->t1 || t1 < l->t0 ) {
                return OUT_SEGMENT;
        } else {
                if ( t0 < l->t0 ) {
                        t0 = t1;
                } else {
                        t0 = t0;
                }
        }

        point_on_line3d ( l, t0, p );

        /* Out of the boundary of height [h0, h1] */
        if ( !in_interval ( p->z, pa->h0, pa->h1 ) ) {
                return OUT_SEGMENT;
        }
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* Find line-hyperboloid intersection and compute bias of the intersect
 * Note that the computation is performed at local coordinate system
 * f(x, y, z) = x^2/a^2 + y^2/a^2 - z^2/c^2 = 1 */
int intersect_line_hyperboloid3d ( struct line3d *l, struct hyperboloid3d *hy,
                                   float *t, struct point3d *p, float *bias )
{
        /* 1/a^2*((Px + Vx*t - x0)^2 + (Py + Vy*t - y0)^2) - 1/c^2*(Pz + Vz*t - z0)^2 = 1 */
        float a = hy->a2*(fsqr (l->v.x) + fsqr (l->v.y)) - hy->c2*fsqr (l->v.z);
        float b = 2.0f*(hy->a2*(l->v.x*l->p0.x + l->v.y*l->p0.y) - hy->c2*(l->v.z*l->p0.z));
        float c = hy->a2*(fsqr (l->p0.x) + fsqr (l->p0.y)) - hy->c2*l->p0.z - 1.0f;

        float t0, t1;
        if ( !solve_quadratic ( a, b, c, &t0, &t1 ) ) {
                return OUT_SEGMENT;
        }

        if ( t1 < t0 ) {
                swap ( t0, t1 );
        }
        /* Test if both solution are out of bound ? if not, take one valid */
        if ( t0 > l->t1 || t1 < l->t0 ) {
                return OUT_SEGMENT;
        } else {
                if ( t0 < l->t0 ) {
                        t0 = t1;
                } else {
                        t0 = t0;
                }
        }

        point_on_line3d ( l, t0, p );

        /* Out of the boundary of z-axis */
        if ( !in_interval ( p->z, hy->z0, hy->z1 ) ) {
                return OUT_SEGMENT;
        }
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* Find line-triangle intersection and compute bias of the intersect
 * The computation is performed at barycentric coordinate system
 * regardless of which coordinate system the triangle is in
 * Pt = b0*P0 + b1*P1 + b2*P2, where b0 = 1 - b1 - b2 */
int intersect_line_triangle3d ( struct line3d *l, struct triangle3d *tri,
                                float *t, struct point3d *p, float *bias )
{
        /* b0 = 1 - b1 - b2
         * (1 - b1 - b2)*P0 + b1*P1 + b2*P2 = Pr0 + Vt
         * -Vx*t + b1*(P1x - P0x) + b2*(P2x - P0x) = Pr0x - P0x
         * Substitution: va = P1 - P0, vb = P2 - P0, vc = Pr0 - P0 */
        struct vector3d va, vb, vc;
        sub_vector3d ( &tri->v[1], &tri->v[0], &va );
        sub_vector3d ( &tri->v[2], &tri->v[0], &vb );
        sub_vector3d ( &l->p0, &tri->v[0], &vc );

        /* Solve for t, b1, b2, where
         * 0 <= b0 <= 1 and 0 <= b1 <= 1 and 0 <= b2 <= 1
         * or, b1 + b2 <= 1 and 0 <= b1 <= 1 and b2 => 0
         * which means that the intersect is within the triangle */
        struct vector3d e1, e2;
        cross_vector3d ( &l->v, &vb, &e2 );
        float det = dot_vector3d ( &va, &e2 );
        if ( det == 0.0f ) {
                return OUT_SEGMENT;
        }

        float inv = 1.0f/det;
        float b1 = dot_vector3d ( &vc, &e2 )*inv;
        if ( !in_interval ( b1, 0.0, 1.0f ) ) {
                return OUT_SEGMENT;
        }

        cross_vector3d ( &vc, &va, &e1 );
        float b2 = dot_vector3d ( &l->v, &e1 )*inv;
        if ( b2 < 0.0f || b1 + b2 > 1.0f ) {
                return OUT_SEGMENT;
        }

        // Within the ray ?
        float t0 = dot_vector3d ( &vb, &e1 )*inv;
        if ( !in_interval ( t0, l->t0, l->t1 ) ) {
                return OUT_SEGMENT;
        }

        point_on_line3d ( l, t0, p );
        *t = t0;
        *bias = EPSILON_E5*t0;
        return IN_SEGMENT;
}

/* find ray-triangle intersection and compute bias of the intersect
 * the computation is performed at barycentric coordinate system
 * independent of the coordinate system which the triangle is in
 * Pt = b0*P0 + b1*P1 + b2*P2, where b0 = 1 - b1 - b2 */
bool intersect_ray_triangle3d ( struct ray3d* l,
                                struct point3d* v0, struct point3d* v1, struct point3d* v2,
                                float b[3], float* t, float *bias )
{
        /* b0 = 1 - b1 - b2
         * (1 - b1 - b2)*P0 + b1*P1 + b2*P2 = Pr0 + Vt
         * -Vx*t + b1*(P1x - P0x) + b2*(P2x - P0x) = Pr0x - P0x
         * Substitution: va = P1 - P0, vb = P2 - P0, vc = Pr0 - P0 */
        struct vector3d va, vb, vc;
        vector3d_comps(va.v[i] = v1->p[i] - v0->p[i]);
        vector3d_comps(vb.v[i] = v2->p[i] - v0->p[i]);
        vector3d_comps(vc.v[i] = l->p0.p[i] - v0->p[i]);

        /* Solve for t, b1, b2, where
         * 0 <= b0 <= 1 and 0 <= b1 <= 1 and 0 <= b2 <= 1
         * or, b1 + b2 <= 1 and 0 <= b1 <= 1 and b2 => 0
         * which means that the intersect is within the triangle */
        struct vector3d e1, e2;
        cross_vector3d(&l->v, &vb, &e2);

        float det = dot_vector3d(&va, &e2);
        if (det == 0.0f) {
                return false;
        }

        float inv = 1.0f/det;
        float b1 = dot_vector3d(&vc, &e2)*inv;
        if (b1 < 0.0f || b1 > 1.0f) {
                return false;
        }

        cross_vector3d(&vc, &va, &e1);
        float b2 = dot_vector3d(&l->v, &e1)*inv;
        if (b2 < 0.0f || b1 + b2 > 1.0f) {
                return false;
        }

        // Within the ray ?
        float t0 = dot_vector3d(&vb, &e1)*inv;
        if (!in_interval(t0, l->t0, l->t1)) {
                return false;
        }

        b[0] = 1.0f - b1 - b2;
        b[1] = b1;
        b[2] = b2;

        *t = t0;
        *bias = 1e-5f*t0;
        return true;
}

/* Find line-box intersection
 * Computation can be performed at any coordinate system
 * P0 + V*t0 = MIN and P0 + V*t1 = MAX */
bool intersect_line_box3d ( struct ray3d *r, struct box3d *b, float *t0, float *t1 )
{
        float min = r->t0;
        float max = r->t1;

        /* Test against the plane that parallels to x, y, z axis respectively */
        int i;
        for ( i = 0; i < 3; i ++ ) {
                float inv = 1.0f/r->v.v[i];
                float k0 = (b->min.p[i] - r->p0.p[i])*inv;
                float k1 = (b->max.p[i] - r->p0.p[i])*inv;
                if ( k0 > k1 ) {
                        /* Marching toward the middle of the ray */
                        min = max ( k1, min );
                        max = min ( k0, max );
                } else {
                        /* Marching toward the middle of the ray */
                        min = max ( k0, min );
                        max = min ( k1, max );
                }
                /* Outside the corner :) */
                if ( min > max ) {
                        return false;
                }
        }
        *t0 = min;
        *t1 = max;
        return true;
}

bool occlude_line_box3d ( struct ray3d* r, struct box3d* b )
{
        float min = r->t0;
        float max = r->t1;

        /* Test against the plane that parallels to x, y, z axis respectively */
        int i;
        for ( i = 0; i < 3; i ++ ) {
                float inv = 1.0f/r->v.v[i];
                float k0 = (b->min.p[i] - r->p0.p[i])*inv;
                float k1 = (b->max.p[i] - r->p0.p[i])*inv;
                if ( k0 > k1 ) {
                        /* Marching toward the middle of the ray */
                        min = max ( k1, min );
                        max = min ( k0, max );
                } else {
                        /* Marching toward the middle of the ray */
                        min = max ( k0, min );
                        max = min ( k1, max );
                }
                /* Outside the corner :) */
                if ( min > max ) {
                        return false;
                }
        }
        return true;
}

void build_box3d ( struct point3d *p0, struct point3d *p1, struct box3d *b )
{
        b->min.x = min ( p0->x, p1->x );
        b->min.y = min ( p0->y, p1->y );
        b->min.z = min ( p0->z, p1->z );

        b->max.x = max ( p0->x, p1->x );
        b->max.y = max ( p0->y, p1->y );
        b->max.z = max ( p0->z, p1->z );
}

void init_box3d ( struct box3d *b )
{
        set_point3d ( FLOAT_MAX, FLOAT_MAX, FLOAT_MAX, &b->min );
        set_point3d ( -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX, &b->max );
}

void copy_box3d ( struct box3d *b0, struct box3d *b1 )
{
        copy_point3d ( &b0->min, &b1->min );
        copy_point3d ( &b0->max, &b1->max );
}

void union_box3d ( struct box3d *b0, struct box3d *b1, struct box3d *u )
{
        u->min.x = min ( b0->min.x, b1->min.x );
        u->min.y = min ( b0->min.y, b1->min.y );
        u->min.z = min ( b0->min.z, b1->min.z );

        u->max.x = max ( b0->max.x, b1->max.x );
        u->max.y = max ( b0->max.y, b1->max.y );
        u->max.z = max ( b0->max.z, b1->max.z );

}

/* Expand the box to cover the point */
void union_box3d_p ( struct box3d *b, struct point3d *p, struct box3d *u )
{
        u->min.x = min ( b->min.x, p->x );
        u->min.y = min ( b->min.y, p->y );
        u->min.z = min ( b->min.z, p->z );

        u->max.x = max ( b->max.x, p->x );
        u->max.y = max ( b->max.y, p->y );
        u->max.z = max ( b->max.z, p->z );
}

float surface_area_box3d ( struct box3d *b )
{
        struct vector3d d;
        build_vector3d ( &b->min, &b->max, &d );
        return 2.0f*(d.x*d.y + d.x*d.z + d.y*d.z);
}

float volume_box3d ( struct box3d *b )
{
        struct vector3d d;
        build_vector3d ( &b->min, &b->max, &d );
        return d.x*d.y*d.z;
}

void translate_box3d ( struct box3d *b, struct matrix4x4 *t, struct box3d *br )
{
        mul_point3d_matrix4x4 ( &b->min, t, &br->min );
        mul_point3d_matrix4x4 ( &b->max, t, &br->max );
}

/* Apply general transform to a box */
void transform_box3d ( struct box3d *b, struct matrix4x4 *t, struct box3d *br )
{
        struct box3d tmp;
        mul_point3d_matrix4x4 ( &b->min, t, &tmp.min );
        mul_point3d_matrix4x4 ( &b->max, t, &tmp.max );
        correct_box3d ( &tmp, br );
}

/* Correct the min-point and max-point position */
void correct_box3d ( struct box3d *b, struct box3d *b_out )
{
        build_box3d ( &b->min, &b->max, b_out );
}

float surface_area_sphere3d ( struct sphere3d *sp )
{
        return (sp->z1 - sp->z0)*sp->r;
}

float surface_area_cylinder3d ( struct cylinder3d *cy )
{
        return (cy->z1 - cy->z0)*cy->r;
}

float surface_area_disk3d ( struct disk3d *d )
{
        return 0.5f*(fsqr (d->r) - fsqr (d->ir));
}

float surface_area_cone3d ( struct cone3d *c )
{
        return fsqr (c->h)*sqrtf (fsqr (c->h) + fsqr (c->r))/(2.0f*c->r);
}

float surface_area_paraboloid3d ( struct paraboloid3d *pa )
{
        return 1.0f/12.0f*(powf (1.0f + 4.0f*pa->h0, 1.5f) -
                           powf (1.0f + 4.0f*pa->h1, 1.5f));
}

float surface_area_hyperboloid3d ( struct hyperboloid3d *hy )
{
        return 1.0f/6.0f*
               (2.0f*(fquad (hy->p0.x) - fcube (hy->p0.x)*hy->p1.x + fquad (hy->p1.x) +
                      fsqr (hy->p0.y) + hy->p0.y*hy->p1.y + fsqr (hy->p1.y))*
                (fsqr (hy->p0.y - hy->p1.y) + fsqr (hy->p0.z - hy->p1.z)) +
                fsqr (hy->p1.x)*(hy->p0.z - hy->p1.z)*
                (5.0f*fsqr (hy->p0.y) + 2.0f*hy->p0.y*hy->p1.y -
                 4.0f*fsqr (hy->p1.y) + 2.0f*hy->p0.z - hy->p1.z) +
                fsqr (hy->p0.x)*
                (-4.0f*fsqr (hy->p0.y) + 2.0f*hy->p0.y*hy->p1.y +
                 5.0f*fsqr (hy->p1.y) + 2.0f*fsqr (hy->p0.z - hy->p1.z)) -
                2.0f*hy->p0.x*hy->p1.x*
                (fsqr (hy->p1.x) - fsqr (hy->p0.y) + 5.0f*hy->p0.y*hy->p1.y -
                 fsqr (hy->p1.y) - fsqr (hy->p0.z) +
                 2.0f*hy->p0.z*hy->p1.z - fsqr (hy->p1.z)));
}

float surface_area_triangle3d ( struct triangle3d *t )
{
        /* A = 1/2*|a×b| = 1/2*|a||b|*sin α */
        struct vector3d a, b;
        build_vector3d ( &t->v[0], &t->v[1], &a );
        build_vector3d ( &t->v[0], &t->v[2], &b );

        struct vector3d s2;
        cross_vector3d ( &a, &b, &s2 );
        return 0.5f*length_vector3d ( &s2 );
}

void box_sphere3d ( struct sphere3d *sp, struct box3d *b )
{
        b->min.x = -sp->r;
        b->min.y = -sp->r;
        b->min.z = sp->z0;

        b->min.x = sp->r;
        b->min.y = sp->r;
        b->min.z = sp->z1;
}

void box_cylinder3d ( struct cylinder3d *cy, struct box3d *b )
{
        b->min.x = -cy->r;
        b->min.y = -cy->r;
        b->min.z = cy->z0;

        b->max.x = cy->r;
        b->max.y = cy->r;
        b->max.z = cy->z1;
}

void box_disk3d ( struct disk3d *d, struct box3d *b )
{
        b->min.x = -d->r;
        b->min.y = -d->r;
        b->min.z = d->h;

        b->max.x = d->r;
        b->max.y = d->r;
        b->max.z = d->h;
}

void box_cone3d ( struct cone3d *c, struct box3d *b )
{
        b->min.x = -c->r;
        b->min.y = -c->r;
        b->min.z = 0.0f;

        b->max.x = c->r;
        b->max.y = c->r;
        b->max.z = c->h;
}

void box_paraboloid3d ( struct paraboloid3d *pa, struct box3d *b )
{
        b->min.x = -pa->r;
        b->min.y = -pa->r;
        b->min.z = pa->h0;

        b->max.x = pa->r;
        b->max.y = pa->r;
        b->max.z = pa->h1;
}

void box_hyperboloid3d ( struct hyperboloid3d *hy, struct box3d *b )
{
        b->min.x = -hy->r;
        b->min.y = -hy->r;
        b->min.z = hy->z0;

        b->max.x = hy->r;
        b->max.y = hy->r;
        b->max.z = hy->z1;
}

void box_triangle3d ( struct triangle3d *tri, struct box3d *b )
{
        b->min.x = min ( min ( tri->v[0].x, tri->v[1].x ), tri->v[2].x );
        b->min.y = min ( min ( tri->v[0].y, tri->v[1].y ), tri->v[2].y );
        b->min.z = min ( min ( tri->v[0].z, tri->v[1].z ), tri->v[2].z );

        b->max.x = max ( max ( tri->v[0].x, tri->v[1].x ), tri->v[2].x );
        b->max.y = max ( max ( tri->v[0].y, tri->v[1].y ), tri->v[2].y );
        b->max.z = max ( max ( tri->v[0].z, tri->v[1].z ), tri->v[2].z );
}

/* Convert the spherical coordinate to descartes' */
void spherical_to_vector3d ( struct spherical3d *s, struct vector3d *v )
{
        float sin_the = sinf ( s->the );
        float cos_the = cosf ( s->the );
        float sin_phi = sinf ( s->phi );
        float cos_phi = cosf ( s->phi );
        v->x = s->r*sin_the*cos_phi;
        v->y = s->r*sin_the*sin_phi;
        v->z = s->r*cos_the;
}

/* This version produces an normalized unit vector */
void spherical_to_vector3d_n ( struct spherical3d *s, struct vector3d *v )
{
        float sin_the = sinf ( s->the );
        float cos_the = cosf ( s->the );
        float sin_phi = sinf ( s->phi );
        float cos_phi = cosf ( s->phi );
        v->x = sin_the*cos_phi;
        v->y = sin_the*sin_phi;
        v->z = cos_the;
}

/* Convert the spherical coordinate to an arbitary basis */
void spherical_to_vector3d_a ( struct spherical3d *s,
                               struct vector3d *u, struct vector3d *v, struct vector3d *n,
                               struct vector3d *v_out )
{
        float sin_the = sinf ( s->the );
        float cos_the = cosf ( s->the );
        float sin_phi = sinf ( s->phi );
        float cos_phi = cosf ( s->phi );
        float x = s->r*sin_the*cos_phi;
        float y = s->r*sin_the*sin_phi;
        float z = s->r*cos_the;
        scale_vector3d ( x, u, v_out );
        sscale_add_vector3d_u ( v_out, y, v );
        sscale_add_vector3d_u ( v_out, z, n );
}

/* Convert the normalized spherical coordinate to an arbitary basis */
void spherical_to_vector3d_na ( struct spherical3d *s,
                                struct vector3d *u, struct vector3d *v, struct vector3d *n,
                                struct vector3d *v_out )
{
        float sin_the = sinf ( s->the );
        float cos_the = cosf ( s->the );
        float sin_phi = sinf ( s->phi );
        float cos_phi = cosf ( s->phi );
        float x = sin_the*cos_phi;
        float y = sin_the*sin_phi;
        float z = cos_the;
        scale_vector3d ( x, u, v_out );
        sscale_add_vector3d_u ( v_out, y, v );
        sscale_add_vector3d_u ( v_out, z, n );
}

/* This version produces a non-unit spherical coordinate */
void vector_to_spherical3d ( struct vector3d *v, struct spherical3d *s )
{
        s->r = length_vector3d ( v );
        s->the = acosf ( v->z/s->r );
        s->phi = arctan2f ( v->y, v->x );
        s->phi = (s->phi < 0.0f) ? (s->phi + PI2) : s->phi;
}

/* This version requires a normalized vector and produces a unit spherical coordinate */
void vector_to_spherical3d_n ( struct vector3d *v, struct spherical3d *s )
{
        s->r = 1.0f;
        s->the = acosf ( v->z );
        s->phi = arctan2f ( v->y, v->x );
        s->phi = (s->phi < 0.0f) ? (s->phi + PI2) : s->phi;
}
