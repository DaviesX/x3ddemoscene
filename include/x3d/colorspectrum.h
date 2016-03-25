#ifndef COLORSPECTRUM_H_INCLUDED
#define COLORSPECTRUM_H_INCLUDED


#include <math/math.h>

/*
 * Definitions
 */

#define declare_color_reserve( _name )		(untyped _name[4*4])

#define SPECTRUM_MIN_LAMBDA			360
#define SPECTRUM_MAX_LAMBDA			830
#define SPECTRUM_MATCHING_MAX_SAMPLES		(SPECTRUM_MAX_LAMBDA - SPECTRUM_MIN_LAMBDA + 1)
#define SPECTRUM_MAX_LAMBDA_SAMPLES		471


/*
 * Structures
 */

/* rgb or xyz floatting point color */
struct float_color3 {
        union {
                float rgb[3];
                float xyz[3];
                float c[3];
                struct {
                        float r;
                        float g;
                        float b;
                };
                struct {
                        float x;
                        float y;
                        float z;
                };
        };
};

/* floating point color for ARGB */
struct float_color4 {
        union {
                float rgba[4];
                struct {
                        float r;
                        float g;
                        float b;
                        float a;
                };
        };
};

struct vector3d;

/*
 * Function's declarations
 */

void init_color3 ( float c, struct float_color3 *c3 );
void init_color4 ( float c, struct float_color4 *c4 );
void set_color3 ( float x, float y, float z, struct float_color3 *c3 );
void set_color4 ( float r, float g, float b, float a, struct float_color4 *c4 );

void add_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out );
void sub_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out );
void mul_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out );
void div_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out );
void sqrt_color3 ( struct float_color3 *c, struct float_color3 *c_out );
void pow_color3 ( struct float_color3 *c, struct vector3d *e, struct float_color3 *c_out );
void scale_color3 ( float k, struct float_color3 *c, struct float_color3 *c_out );
void sscale_add_color3 ( struct float_color3 *c0, float k, struct float_color3 *c1,
                         struct float_color3 *c_out );

int is_color_equal ( struct float_color3 *c0, struct float_color3 *c1 );
int is_color_black ( struct float_color3 *c );
void clamp_color ( struct float_color3 *c, float l, float h, struct float_color3 *c_out );

void xyz_to_rgb ( struct float_color3 *xyz, struct float_color3 *rgb );
void rgb_to_xyz ( struct float_color3 *rgb, struct float_color3 *xyz );
void color3_to_color4 ( struct float_color3 *c3, struct float_color4 *c4 );

#define color3_comps(_CODE)             \
{                                       \
        int i;                          \
        for (i = 0; i < 3; i ++) {      \
                _CODE;                  \
        }                               \
}


#endif // COLORSPECTRUM_H_INCLUDED
