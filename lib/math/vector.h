#ifndef X3DVECTOR_H_INCLUDED
#define X3DVECTOR_H_INCLUDED


/*
 * Definitions
 */

#define VECTOR_X_AXIS		0
#define VECTOR_Y_AXIS		1
#define VECTOR_Z_AXIS		2


/*
 * Structures
 */

/* Points and vectors under cartesian coordinate or homogenuous coordinate */
struct vector2d {
        union {
                float v[2];
                float p[2];

                struct {
                        float x;	// coordinate x
                        float y;	// coordinate y
                };
        };
};

struct ivector2d {
        union {
                int v[2];
                int p[2];

                struct {
                        int x;	// coordinate x
                        int y;	// coordinate y
                };
        };
};

struct vector3d {
        union {
                float v[3];
                float p[3];

                struct {
                        float x;	// coordinate x
                        float y;	// coordinate y
                        float z;	// coordinate z
                };
        };
};

struct ivector3d {
        union {
                int v[3];
                int p[3];

                struct {
                        int x;	// coordinate x
                        int y;	// coordinate y
                        int z;	// coordinate z
                };
        };
};

struct vector4d {
        union {
                float v[4];
                float p[4];

                struct {
                        float x;	// coordinate x
                        float y;	// coordinate y
                        float z;	// coordinate z
                        float w;	// homogeneous coordinate
                };
        };
};

struct ivector4d {
        union {
                int v[4];
                int p[4];

                struct {
                        int x;	// coordinate x
                        int y;	// coordinate y
                        int z;	// coordinate z
                        int w;	/* !homogeneous coordinate still in float point */
                };
        };
};

#define point2d 	vector2d
#define ipoint2d 	ivector2d
#define point3d		vector3d
#define ipoint3d	ivector3d
#define point4d		vector4d
#define ipoint4d	ivector4d

/*
 * Functions' definition
 */

void add_vector2d ( struct vector2d *v0, struct vector2d *v1, struct vector2d *v_out );
void add_vector3d ( struct vector3d *v0, struct vector3d *v1, struct vector3d *v_out );
void add_vector4d ( struct vector4d *v0, struct vector4d *v1, struct vector4d *v_out );

void sub_vector2d ( struct vector2d *v0, struct vector2d *v1, struct vector2d *v_out );
void sub_vector3d ( struct vector3d *v0, struct vector3d *v1, struct vector3d *v_out );
void sub_vector4d ( struct vector4d *v0, struct vector4d *v1, struct vector4d *v_out );

void inv_vector2d ( struct vector2d *v, struct vector2d *v_out );
void inv_vector3d ( struct vector3d *v, struct vector3d *v_out );
void inv_vector4d ( struct vector4d *v, struct vector4d *v_out );

void scale_vector2d ( float k, struct vector2d *v, struct vector2d *v_out );
void scale_vector3d ( float k, struct vector3d *v, struct vector3d *v_out );
void scale_vector4d ( float k, struct vector4d *v, struct vector4d *v_out );

void sscale_add_vector2d ( struct vector2d *v0, float k, struct vector2d *v1, struct vector2d *v_out );
void sscale_add_vector3d ( struct vector3d *v0, float k, struct vector3d *v1, struct vector3d *v_out );
void sscale_add_vector4d ( struct vector4d *v0, float k, struct vector4d *v1, struct vector4d *v_out );

void dscale_add_vector2d ( float k0, struct vector2d *v0, float k1, struct vector2d *v1, struct vector2d *v_out );
void dscale_add_vector3d ( float k0, struct vector3d *v0, float k1, struct vector3d *v1, struct vector3d *v_out );
void dscale_add_vector4d ( float k0, struct vector4d *v0, float k1, struct vector4d *v1, struct vector4d *v_out );

float dot_vector2d ( struct vector2d *v0, struct vector2d *v1 );
float dot_vector3d ( struct vector3d *v0, struct vector3d *v1 );
float dot_vector4d ( struct vector4d *v0, struct vector4d *v1 );

float cross_vector2d ( struct vector2d *v0, struct vector2d *v1 );
void cross_vector3d ( struct vector3d *v0, struct vector3d *v1, struct vector3d *vec_orth );
void cross_vector4d ( struct vector4d *v0, struct vector4d *v1, struct vector4d *vec_orth );

float length_vector2d ( struct vector2d *v );
float length_vector3d ( struct vector3d *v );
float length_vector4d ( struct vector4d *v );

void normalize_vector2d ( struct vector2d *v, struct vector2d *vec_out );
void normalize_vector3d ( struct vector3d *v, struct vector3d *vec_out );
void normalize_vector4d ( struct vector4d *v, struct vector4d *vec_out );

void init_vector2d ( struct vector2d *v );
void init_vector3d ( struct vector3d *v );
void init_vector4d ( struct vector4d *v );
void init_point4d ( struct point4d *p );

void set_vector2d ( float x, float y, struct vector2d *v );
void set_vector3d ( float x, float y, float z, struct vector3d *v );
void set_vector4d ( float x, float y, float z, float w, struct vector4d *v );

void set_unit_vector2d ( float x, float y, struct vector2d *v );
void set_unit_vector3d ( float x, float y, float z, struct vector3d *v );
void set_unit_vector4d ( float x, float y, float z, float w, struct vector4d *v );

void div_w_point4d ( struct point4d *p );
void div_w_point4d_p ( struct point4d *p );
void point4d_to_point3d ( struct point4d *p_homo, struct point3d *p_carte );

void copy_vector2d ( struct vector2d *v_src, struct vector2d *v_dest );
void copy_vector3d ( struct vector3d *v_src, struct vector3d *v_dest );
void copy_vector4d ( struct vector4d * v_src, struct vector4d * v_dest );

void build_vector2d ( struct point2d *p_init, struct point2d *p_end, struct vector2d *v_out );
void build_vector3d ( struct point3d *p_init, struct point3d *p_end, struct vector3d *v_out );
void build_vector4d ( struct point4d *p_init, struct point4d *p_end, struct vector4d *v_out );

float cos_vector2d ( struct vector2d *v0, struct vector2d *v1 );
float cos_vector3d ( struct vector3d *v0, struct vector3d *v1 );
float cos_vector4d ( struct vector4d *v0, struct vector4d *v1 );

void reflect_vector2d(struct vector2d *normal, struct vector2d *incident, struct vector2d *vec_refl);
void reflect_vector3d(struct vector3d *normal, struct vector3d *incident, struct vector3d *vec_refl);
void reflect_vector3d_r(struct vector3d *normal, struct vector3d *incident, struct vector3d *vec_refl);
void reflect_vector4d ( struct vector4d *normal, struct vector4d *incident, struct vector4d *vec_refl );
void reflect_cos_vector2d ( float cos, struct vector2d *normal,
                            struct vector2d *incident, struct vector2d *vec_refl );
void reflect_cos_vector3d ( float cos, struct vector3d *normal,
                            struct vector3d *incident, struct vector3d * vec_refl );
void reflect_cos_vector4d ( float cos, struct vector4d *normal,
                            struct vector4d *incident, struct vector4d *vec_refl );

void print_vector2d ( struct vector2d *v );
void print_vector3d ( struct vector3d *v );
void print_vector4d ( struct vector4d *v );


/*
 * Definitions
 */

/* Utility macro functions */
#define clear_vector2d( _v ) 			(memset ( _v, 0, sizeof ( struct vector2d ) ))
#define clear_vector3d( _v ) 			(memset ( _v, 0, sizeof ( struct vector3d ) ))
#define clear_vector4d( _v ) 			(memset ( _v, 0, sizeof ( struct vector4d ) ))

#define to_3d( _v )				((struct vector3d *) (_v))
#define to_4d( _v )				((struct vector4d *) (_v))

/* Simplified unary operations */
#define add_vector2d_u( _v0, _v1 )		(add_vector2d ( _v0, _v1, _v0 ))
#define add_vector3d_u( _v0, _v1 )		(add_vector3d ( _v0, _v1, _v0 ))
#define add_vector4d_u( _v0, _v1 )		(add_vector4d ( _v0, _v1, _v0 ))

#define sub_vector2d_u( _v0, _v1 )		(sub_vector2d ( _v0, _v1, _v0 ))
#define sub_vector3d_u( _v0, _v1 )		(sub_vector3d ( _v0, _v1, _v0 ))
#define sub_vector4d_u( _v0, _v1 )		(sub_vector4d ( _v0, _v1, _v0 ))

#define inv_vector2d_u( _v )			(inv_vector2d ( _v, _v ))
#define inv_vector3d_u( _v )			(inv_vector3d ( _v, _v ))
#define inv_vector4d_u( _v )			(inv_vector4d ( _v, _v ))

#define scale_vector2d_u( _k, _v )		(scale_vector2d ( _k, _v, _v ))
#define scale_vector3d_u( _k, _v )		(scale_vector3d ( _k, _v, _v ))
#define scale_vector4d_u( _k, _v )		(scale_vector4d ( _k, _v, _v ))

#define sscale_add_vector2d_u( _v0, _k, _v1 )	(sscale_add_vector2d ( _v0, _k, _v1, _v0 ))
#define sscale_add_vector3d_u( _v0, _k, _v1 )	(sscale_add_vector3d ( _v0, _k, _v1, _v0 ))
#define sscale_add_vector4d_u( _v0, _k, _v1 )	(sscale_add_vector4d ( _v0, _k, _v1, _v0 ))

#define normalize_vector2d_u( _v )		(normalize_vector2d ( _v, _v ))
#define normalize_vector3d_u( _v )		(normalize_vector3d ( _v, _v ))
#define normalize_vector4d_u( _v )		(normalize_vector4d ( _v, _v ))

/* Reference name for point operations */
#define copy_point2d			copy_vector2d
#define copy_point3d			copy_vector3d
#define copy_point4d			copy_vector4d

#define set_point2d			set_vector2d
#define set_point3d			set_vector3d
#define set_point4d			set_vector4d

#define init_point2d			init_vector2d
#define init_point3d			init_vector3d
/** #define init_point4d			init_vector4d  (points differ from vector in homogenuous coordinate) **/

#define add_point2d			add_vector2d
#define add_point3d			add_vector3d
#define add_point4d			add_vector4d
#define add_point2d_u			add_vector2d_u
#define add_point3d_u			add_vector3d_u
#define add_point4d_u			add_vector4d_u

#define sub_point2d			sub_vector2d
#define sub_point3d			sub_vector3d
#define sub_point4d			sub_vector4d
#define sub_point2d_u			sub_vector2d_u
#define sub_point3d_u			sub_vector3d_u
#define sub_point4d_u			sub_vector4d_u

#define inv_point2d			inv_vector2d
#define inv_point3d			inv_vector3d
#define inv_point4d			inv_vector4d
#define inv_point2d_u			inv_vector2d_u
#define inv_point3d_u			inv_vector3d_u
#define inv_point4d_u			inv_vector4d_u

#define scale_point2d			scale_vector2d
#define scale_point3d			scale_vector3d
#define scale_point4d			scale_vector4d
#define scale_point2d_u			scale_vector2d_u
#define scale_point3d_u			scale_vector3d_u
#define scale_point4d_u			scale_vector4d_u

#define sscale_add_point2d		sscale_add_vector2d
#define sscale_add_point3d		sscale_add_vector3d
#define sscale_add_point4d		sscale_add_vector4d
#define sscale_add_point2d_u		sscale_add_vector2d_u
#define sscale_add_point3d_u		sscale_add_vector3d_u
#define sscale_add_point4d_u		sscale_add_vector4d_u
#define dscale_add_point2d		dscale_add_vector2d
#define dscale_add_point3d		dscale_add_vector3d
#define dscale_add_point4d		dscale_add_vector4d

#define clear_point2d			clear_vector2d
#define clear_point3d			clear_vector3d
#define clear_point4d			clear_vector4d


#define vector4d_comps(_CODE)           \
{                                       \
        int i;                          \
        for (i = 0; i < 4; i ++) {      \
                _CODE;                  \
        }                               \
}
#define vector3d_comps(_CODE)           \
{                                       \
        int i;                          \
        for (i = 0; i < 3; i ++) {      \
                _CODE;                  \
        }                               \
}
#define vector2d_comps(_CODE)           \
{                                       \
        int i;                          \
        for (i = 0; i < 2; i ++) {      \
                _CODE;                  \
        }                               \
}

#define point2d_comps                   vector2d_comps
#define point3d_comps                   vector3d_comps
#define point4d_comps                   vector4d_comps


#endif // X3DVECTOR_H_INCLUDED
