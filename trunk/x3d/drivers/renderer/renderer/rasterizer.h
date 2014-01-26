#ifndef RASTERIZER_H_INCLUDED
#define RASTERIZER_H_INCLUDED


#include "rasterization.h"

struct rtcontext;
struct rt_point;
struct rt_line;
struct rt_triangle;

/*
 * structures
 */
/* the following structures and functions are _NOT_ interface */
typedef void (*FragShader) ( struct alg_cdecl_stack *params, void *fo );
typedef int (*TestFunc) ( int x, int y, float inv_w, untyped *buf );

struct rasterizer {
	FragShader shader_func;
	untyped *color_buf;
	untyped *depth_buf;
	untyped *stencil_buf;
	int buf_x, buf_y;
	struct matrix4x4 t_screen;
	TestFunc test_func[3];
	int v_in_off[MAX_VERT_COMP];
	int v_in_format[MAX_VERT_COMP];
	int v_in_size;
	int n_in_comp;
	int f_out_size;
	enum RT_DRAWMODE_IDR draw_mode;
	void (*rasterizer_func) ( void );
};

/*
 * functions' declaration
 */
struct rasterizer *create_rasterizer ( void );
void free_rasterizer ( struct rasterizer *rt );
void rasterizer_finalize ( struct rtcontext *cont, struct rasterizer *rt );
void rasterizer_run ( struct rasterizer *rt, rt_vertex *vert_list,
		      enum RT_PRIMITIVE_TYPE prim_type, int count );


#endif // RASTERIZER_H_INCLUDED
