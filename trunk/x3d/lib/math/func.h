#ifndef X3DELEMENTARY_H_INCLUDED
#define X3DELEMENTARY_H_INCLUDED



/*
 * Functions defintion
 */
void init_sincos_table ( void );
void init_pow_table ( void );
void init_log2_table ( void );
float fast_sin_m ( float theta );
float fast_sin ( float theta );
float fast_cos_m ( float theta );
float fast_cos ( float theta );
float fast_pow ( float x, float y );
float fast_sqrt ( float a );
float lerp_y_x ( struct point2d *p0, struct point2d *p1, float x );
float lerp_y_t ( struct point2d *p0, struct point2d *p1, float t );
int solve_quadratic ( float a, float b, float c, float *x0, float *x1 );
int log2_int_c ( int x );
int log2_int_t ( int x );
int fast_log2 ( int x );
float arctan2f ( float y, float x );


#endif // X3DELEMENTARY_H_INCLUDED
