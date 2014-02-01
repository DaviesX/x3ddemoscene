#ifndef RT_VERTEXPROCESS_INCLUDED
#define RT_VERTEXPROCESS_INCLUDED

#include <x3d/common.h>
#include <algorithm.h>
#include <math/math.h>
#include "rasterization.h"
#include "vibuffer.h"


struct rtcontext;
struct vertdefn;


/*
 * structures
 */
struct vertprocessor {
	void (*shader_func) ( struct alg_cdecl_stack *params, void *vo );
	int n_done;
	int n_total;
	untyped *vert_stream;
	untyped *index_stream;
	int index_size;
	int v_in_off[MAX_VERT_ATTRI];
	int v_in_size;
	int n_in_attri;
	int v_out_off[MAX_VERT_COMP];
	int v_out_format[MAX_VERT_COMP];
	int v_out_size;
	int n_out_comp;
	float cull_factor;
	struct matrix4x4 t_view;
	struct matrix4x4 t_proj;
	rt_vertex *cache;
	rt_vertex *clip_cache;
	void (*vertprocessor_func) ( void );
};

/*
 * functions' declaration
 */
struct vertprocessor *create_vertprocessor ( void );
void free_vertprocessor ( struct vertprocessor *vp );
void vertprocessor_finalize ( struct rtcontext *cont, struct vertprocessor *vp );
int vertprocessor_run ( struct vertprocessor *vp, enum RT_PRIMITIVE_TYPE prim_type,
			int count, rt_vertex **vo );

/* symbol library */
void vertprocessor_symbol_lib ( void );


#endif // RT_VERTEXPROCESS_INCLUDED
