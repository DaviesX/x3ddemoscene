#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED


/*
 * Definitions
 */

// Definitions for special numbers
#define EPSILON_E1 		(float)(1E-1)
#define EPSILON_E2 		(float)(1E-2)
#define EPSILON_E3 		(float)(1E-3)
#define EPSILON_E4 		(float)(1E-4)
#define EPSILON_E5 		(float)(1E-5)
#define EPSILON_E6 		(float)(1E-6)
#define EPSILON_E7 		(float)(1E-7)
#define DEFAULT_PRECISION	(float)(1E-10)
#define FLOAT_MAX    		(float)(3.402823466E+38)
#define FLOAT_INFINITE		0X7F000000
#define FLOAT_NAN		0X7F000003

// PI
#define PI         ((float)3.141592654f)
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f)
#define PI_INV     ((float)0.318309886f)

// Some useful functions
#define min( _x0, _x1 )			(((_x0) < (_x1)) ? (_x0) : (_x1))
#define max( _x0, _x1 )			(((_x0) > (_x1)) ? (_x0) : (_x1))
#define max_triple( _a, _b, _c )	(((_a) > (_b) && (_a) > (_c)) ? (_a) : ((_b) > (_c) ? (_b) : (_c))
#define min_triple( _a, _b, _c )	(((_a) < (_b) && (_a) < (_c)) ? (_a) : ((_b) < (_c) ? (_b) : (_c))
#define sort_triple( _a, _b, _c ) \
{ \
	typeof (_a) _t; \
	if ( (_a) > (_b) ) { _t = (_a); (_a) = (_b); (_b) = _t; } \
	if ( (_a) > (_c) ) { _t = (_a); (_a) = (_c); (_c) = _t; } \
	if ( (_b) > (_c) ) { _t = (_b); (_b) = (_c); (_c) = _t; } \
}

#define swap( _a, _b )	\
{ \
	typeof (_a) _t = (_a); \
	(_a) = (_b); \
	(_b) = _t; \
}

#define clamp( _x, _a, _b )				((_x) > (_a) ? ((_x) < (_b) ? (_x) : (_b)) : (_a))
#define rad_to_deg( _r )				((_r)*180.0f/PI)
#define deg_to_rad( _d )				((_d)*PI/180.0f)
#define rand_range( _min, _max )			((_min) + (rand ()%((_max) - (_min) + 1)))
#define fcmp_abs( _a, _b )				((fabs ( (_a) - (_b) ) < DEFAULT_PRECISION) ? 1 : 0)
#define fcmp_rel_l( _a, _b )				((fabs ( (_a) - (_b) )/(_a) < DEFAULT_PRECISION || \
							  fabs ( (_a) - (_b))/(_b) < DEFAULT_PRECISION) ? 1 : 0)
#define fcmp_rel_s( _a, _b )				((fabs ( (_a) - (_b) )/(_a) < DEFAULT_PRECISION && \
							  fabs ( (_a) - (_b) )/(_b) < DEFAULT_PRECISION) ? 1 : 0)
#define fcmp_one( _a, _b )				((fabs ( (_a) - (_b) ) < 1.0f) ? 1 : 0)
#define fcmp_abs_c( _a, _b, _c )			((fabs ( (_a) - (_b) ) < (_c)) ? 1 : 0)
#define fcmp_rel_lc( _a, _b, _c )			((fabs ( (_a) - (_b) )/a < (_c) || \
							  fabs ( (_a) - (_b))/(_b) < (_c)) ? 1 : 0)
#define fcmp_rel_sc( _a, _b, _c )			((fabs ( (_a) - (_b) )/(_a) < (_c) && \
							  fabs ( (_a) - (_b) )/(_b) < (_c)) ? 1 : 0)
#define set_bit( _a, _b )				((_a) = (_a) | (_b))
#define free_bit( _a, _b )				((_a) = (_a) & (~(_b)))

#define in_interval( _x, _i0, _i1 )			((_x) >= (_i0) && (_x) <= (_i1))
#define uni_interval( _i0, _i1, _j0, _j1, _k0, _k1) \
{ \
	(_k0) = min ( _i0, _j0 ); \
	(_k1) = max ( _i1, _j1 ); \
}
#define int_interval( _i0, _i1, _j0, _j1, _k0, _k1) \
{ \
	(_k0) = max ( _i0, _j0 ); \
	(_k1) = min ( _i1, _j1 ); \
}

#define fsqr( _x )			((_x)*(_x))
#define fcube( _x )			((_x)*(_x)*(_x))
#define fquad( _x )			((_x)*(_x)*(_x)*(_x))

#define mm_to_unit( _x )		((_x)*1e-2f)
#define cm_to_unit( _x )		((_x)*1e-1f)
#define dm_to_unit( _x )		((_x)*1.0f)
#define m_to_unit( _x )			((_x)*1e+1f)

#define align2( _x )		((_x) + 2 - 1) & ~(2 - 1))
#define align4( _x )		((_x) + 4 - 1) & ~(4 - 1))
#define align8( _x )		((_x) + 8 - 1) & ~(8 - 1))
#define align_n( _x, _n )	(((_x) + (_n) - 1) & ~((_n) - 1))


/*
 * Includes relative headers
 */
#include "vector.h"
#include "matrix.h"
#include "quat.h"
#include "geometry.h"
#include "decimal.h"
#include "fx.h"


#define init_math_lib() \
{ \
	init_sincos_table (); \
	init_pow_table (); \
	init_log2_table (); \
}


#endif // MATH_H_INCLUDED
