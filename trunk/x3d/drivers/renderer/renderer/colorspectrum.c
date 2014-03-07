#include <math.h>
#include <math/math.h>
#include "colorspectrum.h"



void init_color3 ( float c, struct float_color3 *c3 )
{
        c3->x = c;
        c3->y = c;
        c3->z = c;
}

void init_color4 ( float c, struct float_color4 *c4 )
{
        c4->r = c;
        c4->g = c;
        c4->b = c;
        c4->a = c;
}

void set_color3 ( float x, float y, float z, struct float_color3 *c3 )
{
        c3->x = x;
        c3->y = y;
        c3->z = z;
}

void set_color4 ( float r, float g, float b, float a, struct float_color4 *c4 )
{
        c4->r = r;
        c4->g = g;
        c4->b = b;
        c4->a = a;
}

void add_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out )
{
        c_out->x = c0->x + c1->x;
        c_out->y = c0->y + c1->y;
        c_out->z = c0->z + c1->z;
}

void sub_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out )
{
        c_out->x = c0->x - c1->x;
        c_out->y = c0->y - c1->y;
        c_out->z = c0->z - c1->z;
}

void mul_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out )
{
        c_out->x = c0->x*c1->x;
        c_out->y = c0->y*c1->y;
        c_out->z = c0->z*c1->z;
}

void div_color3 ( struct float_color3 *c0, struct float_color3 *c1, struct float_color3 *c_out )
{
        c_out->x = c0->x/c1->x;
        c_out->y = c0->y/c1->y;
        c_out->z = c0->z/c1->z;
}

void sqrt_color3 ( struct float_color3 *c, struct float_color3 *c_out )
{
        c_out->x = sqrtf ( c->x );
        c_out->y = sqrtf ( c->y );
        c_out->z = sqrtf ( c->z );
}

void pow_color3 ( struct float_color3 *c, struct vector3d *e, struct float_color3 *c_out )
{
        c_out->x = powf ( c->x, e->x );
        c_out->y = powf ( c->y, e->y );
        c_out->z = powf ( c->z, e->z );
}

void scale_color3 ( float k, struct float_color3 *c, struct float_color3 *c_out )
{
        c_out->x = k*c->x;
        c_out->y = k*c->y;
        c_out->z = k*c->z;
}

void sscale_add_color3 ( struct float_color3 *c0, float k, struct float_color3 *c1,
                         struct float_color3 *c_out )
{
        c_out->x = c0->x + k*c1->x;
        c_out->y = c0->y + k*c1->y;
        c_out->z = c0->z + k*c1->z;
}

int is_color_equal ( struct float_color3 *c0, struct float_color3 *c1 )
{
        float da = (c0->x - c1->x);
        float db = (c0->y - c1->y);
        float dc = (c0->z - c1->z);
        return *(int *) &da | *(int *) &db | *(int *) &dc;
}

int is_color_black ( struct float_color3 *c )
{
        return !(*(int *) &c->x |
                 *(int *) &c->y |
                 *(int *) &c->z);
}

void clamp_color ( struct float_color3 *c, float l, float h, struct float_color3 *c_out )
{
        c_out->x = clamp ( c->x, l, h );
        c_out->y = clamp ( c->y, l, h );
        c_out->z = clamp ( c->z, l, h );
}

/*
 * The weight are the solution*MSPD(lambda) of the equations that values the
 * constants of the scale of the monitor's SPD to actual SPD perceived by human eyes
 * MSPD(lambda) denotes for the SPD of _LCD_ monitor
 */
void xyz_to_rgb ( struct float_color3 *xyz, struct float_color3 *rgb )
{
        rgb->r = XyzToRWeight[0]*xyz->x + XyzToRWeight[1]*xyz->y + XyzToRWeight[2]*xyz->z;
        rgb->r = XyzToGWeight[0]*xyz->x + XyzToGWeight[1]*xyz->y + XyzToGWeight[2]*xyz->z;
        rgb->r = XyzToBWeight[0]*xyz->x + XyzToBWeight[1]*xyz->y + XyzToBWeight[2]*xyz->z;
}

/* These weights are the inverse of the equations */
void rgb_to_xyz ( struct float_color3 *rgb, struct float_color3 *xyz )
{
        xyz->x = RgbToXWeight[0]*rgb->r + RgbToXWeight[1]*rgb->g + RgbToXWeight[2]*rgb->b;
        xyz->y = RgbToYWeight[0]*rgb->r + RgbToYWeight[1]*rgb->g + RgbToYWeight[2]*rgb->b;
        xyz->z = RgbToZWeight[0]*rgb->r + RgbToZWeight[1]*rgb->g + RgbToZWeight[2]*rgb->b;
}

void color3_to_color4 ( struct float_color3 *c3, struct float_color4 *c4 )
{
        c4->r = c3->r;
        c4->g = c3->g;
        c4->b = c3->b;
        c4->a = 1.0f;
}
