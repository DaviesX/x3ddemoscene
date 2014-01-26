/* func.c: All utility math functions go here */
#include "math.h"
#include "fx.h"


/* Global variables */
static float g_sin_tbl[361];
static float g_cos_tbl[361];
static float g_pow_tbl[1001][101];
static int g_log2_tbl[2048];



/* Precompute the value for the fast sin/cos function */
void init_sincos_table ( void )
{
	int i;
	for ( i = 0; i <= 360; i ++ ) {
		g_sin_tbl[i] = sin ( deg_to_rad ( (double) i ) );
		g_cos_tbl[i] = cos ( deg_to_rad ( (double) i ) );
	}
}

/* Precompute the value for the fast pow function defined in [0, 1] */
void init_pow_table ( void )
{
	int ix, iy;
	double x, y;
	ix = 0;
	for ( ix = 0, x = 0.0; x <= 1.0; x += 0.001, ix ++ ) {
		for ( iy = 0, y = 0.0; y <= 1.0; y += 0.01, iy++ ) {
			g_pow_tbl[ix][iy] = pow ( x, y );
		}
	}
}

/* Precompute the value for the fast log2 function */
void init_log2_table ( void )
{
	int ix;
	for ( ix = 0; ix <= 2048; ix ++ ) {
		g_log2_tbl[ix] = floor ( log ( ix )/log ( 2 ) );
	}
}

/* this version would take in angle with modding to 0-360 */
float fast_sin_m ( float theta )
{
	theta = fmodf ( theta, 360.0f );
	if ( theta < 0 ) {
		theta += 360.0;
	}
	int i_theta = (int) theta;
	float frac = theta - i_theta;
	return (g_sin_tbl[i_theta] +
		frac*(g_sin_tbl[i_theta + 1] - g_sin_tbl[i_theta]));
}

/* this version would take in radius */
float fast_sin ( float theta )
{
	theta = rad_to_deg ( theta );
	int i_theta = (int) theta;
	float frac = theta - i_theta;
	return (g_sin_tbl[i_theta] +
		frac*(g_sin_tbl[i_theta + 1] - g_sin_tbl[i_theta]));
}

/* this version would take in angle with modding to 0-360 */
float fast_cos_m ( float theta )
{
	theta = fmodf ( theta, 360.0f );
	if ( theta < 0 ) {
		theta += 360.0;
	}
	int i_theta = (int) theta;
	float frac = theta - i_theta;
	return (g_cos_tbl[i_theta] +
		frac*(g_cos_tbl[i_theta + 1] - g_cos_tbl[i_theta]));
}

/* this version would take in radius */
float fast_cos ( float theta )
{
	theta = rad_to_deg ( theta );
	int i_theta = (int) theta;
	float frac = theta - i_theta;
	return (g_cos_tbl[i_theta] +
		frac*(g_cos_tbl[i_theta + 1] - g_cos_tbl[i_theta]));
}

float fast_pow ( float x, float y )
{
	if ( fabsf ( x ) < EPSILON_E5 ) {
		return 1.0f;
	}

	int ix = (int) (x*1000.0f);
	float dx = x*1000.0f - ix;
	int iy = (int) (y*100.0f);
	float dy = y*100.0f - iy;

	float power0 = g_pow_tbl[ix + 0][iy + 0];
	float power1 = g_pow_tbl[ix + 1][iy + 0];
	float power2 = g_pow_tbl[ix + 0][iy + 1];

	return power0 +
	       (power1 - power0)*dx + (power2 - power0)*dy;
}

float fast_sqrt ( float a )
{
	uint32_t i;
	float x, y;
	const float f = 1.5f;

	x = a*0.5f;
	y = a;
	i = *(uint32_t *) &y;
	i = 0x5f375a86 - (i >> 1);
	y = *(float *) &i;

	y = y*(f - (x*y*y));
	y = y*(f - (x*y*y));
	return a*y;
}

float lerp_y_x ( struct point2d *p0, struct point2d *p1, float x )
{
	return p0->y + (p1->y - p0->y)*
	       (x - p0->x)/(p1->x - p0->x);
}

float lerp_y_t ( struct point2d *p0, struct point2d *p1, float t )
{
	return p0->y + (p1->y - p0->y)*t;
}

int solve_quadratic ( float a, float b, float c, float *x0, float *x1 )
{
	float delta = fsqr ( b ) - 4.0f*a*c;
	if ( delta >= 0.0f ) {
		float q;
		if ( b < 0.0f ) {
			q = -0.5f*(-b - sqrtf ( delta ));
		} else {
			q = -0.5f*(-b + sqrtf ( delta ));
		}
		*x0 = q/a;
		*x1 = c/q;
	} else {
		return 0;
	}
	return 1;
}

int log2_int_c ( int x )
{
	uint32_t shift = x;
	int i_log = 0;
	while ( shift >>= 1 ) {
		i_log ++;
	}
	return i_log + (x != 1 << i_log);
}

int log2_int_t ( int x )
{
	int i_log = 0;
	while ( x >>= 1 ) {
		i_log ++;
	}
	return i_log;
}

/* floor(log2 (x)) where x are defined in [0, 2048] */
int fast_log2 ( int x )
{
	return g_log2_tbl[x];
}

float arctan2f ( float y, float x )
{
	if ( x != 0 ) {
		float inv = atanf ( y/x );
		uint32_t mask = ~((*(uint32_t *) &x) & 0X80000000);
		uint32_t y_sign = (*(uint32_t *) &y) & 0X80000000;
		const float pi = PI;
		const uint32_t bin = *(uint32_t *) &pi;
		return inv + ((bin & mask) | y_sign);
	} else {
		const float pi_div_2 = PI_DIV_2;
		const uint32_t bin = *(uint32_t *) &pi_div_2;
		return bin | ((*(uint32_t *) &y) & 0X80000000);
	}
}
