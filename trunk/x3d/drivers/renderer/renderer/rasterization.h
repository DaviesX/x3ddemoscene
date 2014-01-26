#ifndef X3DRASTERPASS_H_INCLUDED
#define X3DRASTERPASS_H_INCLUDED


#include <math/math.h>


/*
 * Definitions
 */

enum RT_CULL_IDR {
	CULL_IDR_NULL,
	CULL_IDR_BACKFACE,
	CULL_IDR_FONTFACE
};

enum RT_CLIP_IDR {
	CLIP_IDR_PRECISE,
	CLIP_IDR_BROAD
};

enum RT_DRAWMODE_IDR {
	RT_WIREFRAME_MODE,
	RT_SOLID_MODE
};

#define RT_BUFFER_NUM		5

enum RT_BUFFER_IDR {
	RT_VERTEX_BUFFER,
	RT_INDEX_BUFFER,
	RT_COLOR_BUFFER,
	RT_DEPTH_BUFFER,
	RT_STENCIL_BUFFER
};

enum RT_BUFFER_STATE {
	RT_BUFFER_ENABLE = 0X1,
	RT_BUFFER_WRITE = 0X2
};

#define SHADER_STAGE_NUM	2
enum RT_SHADER_STAGE {
	RT_VERTEX_SHADER,
	RT_FRAGMENT_SHADER
};

enum RT_SPEC_IDR {
	RT_BUILTIN_SW,
	RT_OPENGL_HW
};

struct vertex_buffer;
struct index_buffer;
struct surface;
struct shader;
struct vertdefn;
struct vertprocessor;
struct rasterizer;
#define rt_vertex			untyped

/*
 * Structures
 */
enum VERTEX_DEFN_FORMAT {
	VERTEX_DEFN_NULL,
	VERTEX_DEFN_INT8,
	VERTEX_DEFN_INT16,
	VERTEX_DEFN_INT32,
	VERTEX_DEFN_INT64,
	VERTEX_DEFN_INT96,
	VERTEX_DEFN_INT128,
	VERTEX_DEFN_FLOAT32,
	VERTEX_DEFN_FLOAT64,
	VERTEX_DEFN_FLOAT96,
	VERTEX_DEFN_FLOAT128,
	VERTEX_DEFN_INTERPOLATE	= 1 << 16,
	VERTEX_DEFN_CORRECTION 	= 1 << 17
};

#define vertdefn_mask( _format )	((_format) & 0XFF)
#define vertdefn_int( _format )		((_format) & (1 << 16))
#define rt_vertex			untyped

static const int SizeofVertDefn[] = {
	[VERTEX_DEFN_NULL] = 0,
	[VERTEX_DEFN_INT8] = 1,
	[VERTEX_DEFN_INT16] = 2,
	[VERTEX_DEFN_INT32] = 4,
	[VERTEX_DEFN_INT64] = 8,
	[VERTEX_DEFN_INT96] = 12,
	[VERTEX_DEFN_INT128] = 16,
	[VERTEX_DEFN_FLOAT32] = 4,
	[VERTEX_DEFN_FLOAT64] = 8,
	[VERTEX_DEFN_FLOAT96] = 12,
	[VERTEX_DEFN_FLOAT128] = 16
};

#define MAX_VERT_COMP		10
struct vertdefn {
	enum VERTEX_DEFN_FORMAT format[MAX_VERT_COMP];
	int offset[MAX_VERT_COMP];
	int n_comp;
	int vertsize;
};

enum RT_PRIMITIVE_TYPE {
	POINT_PRIMITIVE 	= 1,
	LINE_PRIMITIVE 		= 2,
	TRIANGLE_PRIMITIVE 	= 3
};

/* context instance for creating a rasterization pipeline */
struct rtcontext {
	enum RT_SPEC_IDR spec;
	enum RT_BUFFER_STATE buffer_state[RT_BUFFER_NUM];
	struct vertex_buffer *vbuffer;
	struct index_buffer *ibuffer;
	struct vertdefn vert_defn;
	enum RT_CULL_IDR cull_state;
	enum RT_CLIP_IDR clip_method;
	struct matrix4x4 view;
	struct matrix4x4 proj;
	struct shader *shader[SHADER_STAGE_NUM];
	struct surface *color_buffer;
	struct surface *depth_buffer;
	struct surface *stencil_buffer;
	enum RT_PRIMITIVE_TYPE prim_type;
	enum RT_DRAWMODE_IDR draw_mode;
	struct vertprocessor *vpr;
	struct rasterizer *rtr;
};

/*
 * rtcontext's declaration
 */
void init_rtcontext ( struct rtcontext *rc );
void free_rtcontext ( struct rtcontext *rc );
void reset_rtcontext ( struct rtcontext *rc );
void rtcontext_set_view ( struct matrix4x4 *t, struct rtcontext *rc );
void rtcontext_set_proj ( struct matrix4x4 *t, struct rtcontext *rc );
void rtcontext_bind_vertdefn ( struct vertdefn *defn, struct rtcontext *rc );
void rtcontext_bind_vertex_buffer ( struct vertex_buffer *vb, struct rtcontext *rc );
void rtcontext_bind_index_buffer ( struct index_buffer *ib, struct rtcontext *rc );
void rtcontext_bind_color_buffer ( struct surface *cb, struct rtcontext *rc );
void rtcontext_bind_depth_buffer ( struct surface *db, struct rtcontext *rc );
void rtcontext_bind_stencil_buffer ( struct surface *sb, struct rtcontext *rc );
void rtcontext_cull ( enum RT_CULL_IDR idr, struct rtcontext *rc );
void rtcontext_draw_mode ( enum RT_DRAWMODE_IDR draw_mode, struct rtcontext *rc );
void rtcontext_set_buffer ( enum RT_BUFFER_IDR idr, enum RT_BUFFER_STATE state,
			    struct rtcontext *rc );
void rtcontext_bind_shader ( struct shader *shader, enum RT_SHADER_STAGE stage,
			     struct rtcontext *rc );
void rtcontext_set_spec ( enum RT_SPEC_IDR spec, struct rtcontext *rc );
void rtcontext_finalize_pipeline ( struct rtcontext *rc );

void vertdefn_init ( struct vertdefn *defn );
void vertdefn_set ( int i, enum VERTEX_DEFN_FORMAT format,
		    struct vertdefn *defn );
void vertdefn_fix ( struct vertdefn *defn );
int vertdefn_query_size ( struct vertdefn *defn );

/*
 * rtpass's declaration
 */
void rasterization_run_pipeline ( struct rtcontext *rc, bool async );


#endif // X3DRASTERPASS_H_INCLUDED
