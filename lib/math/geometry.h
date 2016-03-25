#ifndef X3DGEOMETRY_H_INCLUDED
#define X3DGEOMETRY_H_INCLUDED



/* Intersection status */
enum INTERSECT_STATE {
        NO_INTERSECT,
        IN_SEGMENT,
        OUT_SEGMENT,
        INTERSECT_EVERYWHERE
};

/*
 * Structures
 */

/* Pt = P0 + Vt, t∈[0, |P1 - P0|]∩[min, max], |v| = 1 */
struct line2d {
        struct point2d p0;			/* p0 : start point, p1 : end point */
        struct point2d p1;
        struct vector2d v;			/* direction vector of the line segment */
        float t0, t1;
};

struct line3d {
        struct point3d p0;
//        struct point3d p1;
        struct vector3d v;
        float t0, t1;
        float len;
        float inv_len2;
};
#define ray3d     line3d

struct rectangle2d {
        union {
                float v[4];
                struct point2d p[2];
                struct {
                        float x0, y0;
                        float x1, y1;
                };
        };
};

struct irectangle2d {
        union {
                int v[4];
                struct ipoint2d p[2];
                struct {
                        int x0, y0;
                        int x1, y1;
                };
        };
};

/* n (nx, ny, nz), p0 (x0, y0, z0), p (x, y, z)
 * nx*(x - x0) + ny*(y - y0) + nz*(z - z0) = 0 */
struct plane3d {
        struct point3d p0;
        struct vector3d n;
};


/* f(x, y, z) = x^2 + y^2 + z^2 - r^2 = 0
 * x = sin(theta)cos(phi)
 * y = sin(theta)sin(phi)
 * z = cos(theta) */
struct sphere3d {
        float r;			/* radius */
        float z0, z1;		/* horizontal clip */
};

/* f(x, y) = x^2 + x^y - r^2 = 0 */
struct cylinder3d {
        float r;
        float z0, z1;
};

/* f(x, y, z) = h */
struct disk3d {
        float ir;		/* inner radius */
        float r;		/* outer radius */
        float h;		/* height */
};

/* f(x, y, z) = (hx)^2/r^2 + (hy)^2/r^2 - (z - h) = 0 */
struct cone3d {
        float a;
        float r;
        float h;
};

/* f(x, y, z) = (h1/r^2)*(x^2 + y^2) - z = 0 */
struct paraboloid3d {
        float a;
        float r;
        float h0;
        float h1;
};

/* f(x, y, z) = x^2/a^2 + y^2/a^2 - z^2/c^2 = 1 */
struct hyperboloid3d {
        float r;
        float a2, c2;		/* It's actually the reciprocal of a^2 and c^2 */
        float z0, z1;
        struct point3d p0;
        struct point3d p1;
};

struct box3d {
        struct point3d min;
        struct point3d max;
};


/* polar coordinate system
 * P(r, θ )*/
struct poloar2d {
        float r;
        float the;
};


/* cylindrical coordinate system
 * P(r, θ , z) */
struct cylindrical3d {
        float r;
        float the;
        float z;
};

/* spherical coordinate system
 * P(r, θ, φ), θ∈[0,π], φ∈[0,2π] */
struct spherical3d {
        float r;
        float the;
        float phi;
};

struct triangle3d {
        struct point3d v[3];
};

struct triangle4d {
        struct point4d v[3];
};

struct frustum3d {
        float z0, z1;
        float delx, dely;
        float a;
};


/*
 * Functions definition
 */
void build_line2d ( struct point2d *p0, struct point2d *p1, struct line2d *l );
void build_line2d_t ( struct point2d *p0, struct point2d *p1, float t0, float t1, struct line2d *l );
void build_line3d ( struct point3d *p0, struct point3d *p1, struct line3d *l );
void build_line3d_t ( struct point3d *p0, struct point3d *p1, float t0, float t1, struct line3d *l );
void build_plane3d ( struct vector3d *n, struct point3d *p, struct plane3d *plane );
void build_plane3d_n ( struct vector3d *n, struct point3d *p, struct plane3d *plane );
void build_rectangle_2d ( float x0, float x1, float y0, float y1, struct rectangle2d *rect );
void build_irectangle_2d ( int x0, int x1, int y0, int y1, struct irectangle2d *rect );

void point_on_line2d ( struct line2d *l, float t, struct point2d *p );
void point_on_line3d ( struct line3d *l, float t, struct point3d *p );

int intersect_line2d ( struct line2d *l0, struct line2d *l1, float *t0_out, float *t1_out );
float in_plane3d ( struct point3d *p, struct plane3d *plane );
int intersect_line_plane3d ( struct plane3d *pl, struct line3d *l, float *t, struct point3d *p );

void rotate_o_point2d ( struct point2d *p, float theta, struct point2d *p_out );
void rotate_a_point2d ( struct point2d *p, struct point2d *o, float theta, struct point2d *p_out );
void rotate_x_point3d ( struct point3d *p, float theta, struct point3d *p_out );
void rotate_y_point3d ( struct point3d *p, float theta, struct point3d *p_out );
void rotate_z_point3d ( struct point3d *p, float theta, struct point3d *p_out );
void rotate_a_point3d ( struct vector3d *a, struct point3d *p, float theta, struct point3d *p_out );

void build_sphere3d ( float r, float z0, float z1, struct sphere3d *sp );
void build_cylinder3d ( float r, float z0, float z1, struct cylinder3d *cy );
void build_disk3d ( float ir, float r, float h, struct disk3d *d );
void build_cone3d ( float r, float h, struct cone3d *c );
void build_paraboloid3d ( float r, float h0, float h1, struct paraboloid3d *pa );
void build_hyperboloid3d ( struct point3d *p0, struct point3d *p1, float z0, float z1,
                           struct hyperboloid3d *hy );

void build_triangle3d ( struct point3d *p0, struct point3d *p1, struct point3d *p2, struct triangle3d *tri );
void build_triangle4d ( struct point3d *p0, struct point3d *p1, struct point3d *p2, struct triangle4d *tri );
void build_triangle4d_w ( struct point4d *p0, struct point4d *p1, struct point4d *p2, struct triangle4d *tri );

int intersect_line_sphere3d ( struct line3d *l, struct sphere3d *sp,
                              float *t, struct point3d *p, float *bias );
int intersect_line_cylinder3d ( struct line3d *l, struct cylinder3d *cy,
                                float *t, struct point3d *p, float *bias );
int intersect_line_disk3d ( struct line3d *l, struct disk3d *d,
                            float *t, struct point3d *p, float *bias );
int intersect_line_cone3d ( struct line3d *l, struct cone3d *cn,
                            float *t, struct point3d *p, float *bias );
int intersect_line_paraboloid3d ( struct line3d *l, struct paraboloid3d *pa,
                                  float *t, struct point3d *p, float *bias );
int intersect_line_hyperboloid3d ( struct line3d *l, struct hyperboloid3d *hy,
                                   float *t, struct point3d *p, float *bias );
int intersect_line_triangle3d ( struct line3d *l, struct triangle3d *tri,
                                float *t, struct point3d *p, float *bias );
bool intersect_ray_triangle3d ( struct ray3d* l,
                                struct point3d* v0, struct point3d* v1, struct point3d* v2,
                                float b[3], float* t, float *bias );

bool intersect_line_box3d ( struct line3d *l, struct box3d *b, float *t0, float *t1 );
bool occlude_line_box3d ( struct ray3d* l, struct box3d* b );

void build_box3d ( struct point3d *p0, struct point3d *p1, struct box3d *b );
void init_box3d ( struct box3d *b );
void copy_box3d ( struct box3d *b0, struct box3d *b1 );
void union_box3d ( struct box3d *b0, struct box3d *b1, struct box3d *u );
void union_box3d_p ( struct box3d *b, struct point3d *p, struct box3d *u );
float surface_area_box3d ( struct box3d *b );
float volume_box3d ( struct box3d *b );
void translate_box3d ( struct box3d *b, struct matrix4x4 *t, struct box3d *br );
void transform_box3d ( struct box3d *b, struct matrix4x4 *t, struct box3d *br );
void correct_box3d ( struct box3d *b, struct box3d *b_out );

float surface_area_sphere3d ( struct sphere3d *sp );
float surface_area_cylinder3d ( struct cylinder3d *cy );
float surface_area_disk3d ( struct disk3d *d );
float surface_area_cone3d ( struct cone3d *c );
float surface_area_paraboloid3d ( struct paraboloid3d *pa );
float surface_area_hyperboloid3d ( struct hyperboloid3d *hy );
float surface_area_triangle3d ( struct triangle3d *t );

void box_sphere3d ( struct sphere3d *sp, struct box3d *b );
void box_cylinder3d ( struct cylinder3d *cy, struct box3d *b );
void box_disk3d ( struct disk3d *d, struct box3d *b );
void box_cone3d ( struct cone3d *c, struct box3d *b );
void box_paraboloid3d ( struct paraboloid3d *pa, struct box3d *b );
void box_hyperboloid3d ( struct hyperboloid3d *hy, struct box3d *b );
void box_triangle3d ( struct triangle3d *tri, struct box3d *b );

void spherical_to_vector3d_n ( struct spherical3d *s, struct vector3d *v );
void spherical_to_vector3d ( struct spherical3d *s, struct vector3d *v );
void spherical_to_vector3d_na ( struct spherical3d *s,
                                struct vector3d *u, struct vector3d *v, struct vector3d *n,
                                struct vector3d *v_out );
void spherical_to_vector3d_a ( struct spherical3d *s,
                               struct vector3d *u, struct vector3d *v, struct vector3d *n,
                               struct vector3d *v_out );
void vector_to_spherical3d_n ( struct vector3d *v, struct spherical3d *s );
void vector_to_spherical3d ( struct vector3d *v, struct spherical3d *s );

void ray3d_build_t(struct ray3d *self, struct point3d *p0, struct point3d *p1, float t0, float t1);
float ray3d_length(struct ray3d* self);
float ray3d_inv_length2(struct ray3d* self);
void ray3d_direction(struct ray3d* self, struct vector3d* v);

void triangle3d_normal(struct triangle3d* self, struct vector3d* n);

/* Unary operations */
#define union_box3d_u( _b0, _b1 )			(union_box3d ( _b0, _b1, _b0 ))
#define union_box3d_pu( _b, _p )			(union_box3d_p ( _b, _p, _b ))

/* Implicit casting */


#endif // X3DGEOMETRY_H_INCLUDED
