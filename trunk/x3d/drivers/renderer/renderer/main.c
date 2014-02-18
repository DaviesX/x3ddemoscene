/* main.c: Unit tests to all rendering functions go here */
#include <x3d/common.h>
#include <logout.h>
#include <x3d/runtime_debug.h>
#include "main.h"


static void dbg_vertprocessor_add_all ( void );
static void dbg_rasterizer_add_all ( void );

/* entry */
void dbg_renderer_add_all ( void )
{
	static bool first_time = true;
	if ( first_time ) {
		dbg_vertprocessor_add_all ();
		dbg_rasterizer_add_all ();
		first_time = false;
	}
}

/* vertex prcessor's */		#include "vertprocessor.h"
#include "dbg_vertprocessor.h"
#include "rasterization.h"

static void vert_post_process ( struct alg_named_params *global_params );

void dbg_vertprocessor_add_all ( void )
{
	vertprocessor_symbol_lib ();
	struct unit_test ut;
	ut.test_name = "vert_post_process";
	ut.test = vert_post_process;
	ut.pos = DBG_KERNEL_START;
	kernel_unit_test_add ( &ut );
}

static void vert_post_process ( struct alg_named_params *global_params )
{
	struct dbg_vertex {
		struct point4d v;
		int p;
	};
	struct dbg_vertex v[3];
	struct dbg_vertex vo[3];
	struct dbg_vertex tmp_cache[20];
	/* a volume with fov = 90 degree, w/h = 4/3, n = 1.0, f = 100.0 */
	struct matrix4x4 t_all;
	struct matrix4x4 t_view;
	set_matrix4x4 ( &t_all,
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0*4.0/3.0, 0.0, 0.0,
			0.0, 0.0, (100.0 + 1.0)/(100.0 - 1.0), 1.0,
			0.0, 0.0, 2.0*100.0*1.0/(1.0 - 100.0), 0.0 );
	identity_matrix4x4 ( &t_view );
	/* point test */
	set_point4d ( 100.0f, 20.0f, 50.0f, 1.0f, &v[0].v );
	v[0].p = 0;
	set_point4d ( 50.0f, 20.0f, 50.0f, 1.0f, &v[1].v );
	v[1].p = 1;
	set_point4d ( 10.0f, 20.0f, 100.0f, 1.0f, &v[2].v );
	v[2].p = 2;
	int n;
	int i;
	for ( i = 0; i < 3; i ++ ) {
		n = dbg_process_point ( &v[i], &t_all, sizeof v[0], &vo[i] );
		log_normal_dbg ( "v%d: %f, %f, %f, %f, %d point inside",
				 vo[i].p, vo[i].v.x, vo[i].v.y, vo[i].v.z, vo[i].v.w, n );
	}
	/* line test */
	set_point4d ( 100.0f, 0.0f, 80.0f, 1.0f, &v[0].v );
	v[0].p = 0;
	set_point4d ( 100.0f, 20.0f, 120.0f, 1.0f, &v[1].v );
	v[1].p = 1;
	int comp_offset[2] = {0, sizeof v[0].v};
	int comp_format[2] = {
		VERTEX_DEFN_FLOAT4 | VERTEX_DEFN_INTERPOLATE,
		VERTEX_DEFN_INT
	};
	n = dbg_process_line ( v, &t_all, comp_offset, comp_format, 2, sizeof v[0], vo );
	log_normal_dbg ( "v%d: %f, %f, %f, %f",
			 vo[0].p, vo[0].v.x, vo[0].v.y, vo[0].v.z, vo[0].v.w );
	log_normal_dbg ( "v%d: %f, %f, %f, %f, %d point inside",
			 vo[1].p, vo[1].v.x, vo[1].v.y, vo[1].v.z, vo[1].v.w, n );
	/* triangle test */
	set_point4d ( 100.0f, -10.0f, 80.0f, 1.0f, &v[0].v );
	v[0].p = 0;
	set_point4d ( 100.0f, 40.0f, 120.0f, 1.0f, &v[1].v );
	v[1].p = 1;
	set_point4d ( 150.0f, 0.0f, 120.0f, 1.0f, &v[2].v );
	v[2].p = 2;
	n = dbg_process_triangle ( v, &t_view, &t_all, 1.0f, comp_offset, comp_format,
				   2, sizeof v[0], tmp_cache, vo );
	log_normal_dbg ( "v%d: %f, %f, %f, %f",
			 vo[0].p, vo[0].v.x, vo[0].v.y, vo[0].v.z, vo[0].v.w );
	log_normal_dbg ( "v%d: %f, %f, %f, %f",
			 vo[1].p, vo[1].v.x, vo[1].v.y, vo[1].v.z, vo[1].v.w, n );
	log_normal_dbg ( "v%d: %f, %f, %f, %f, %d point inside",
			 vo[2].p, vo[2].v.x, vo[2].v.y, vo[2].v.z, vo[22].v.w, n );
}

/* rasterizer's */		#include "rasterizer.h"
void dbg_rasterizer_add_all ( void )
{
}

#if 0
// Test to draw things onto the frame surface
void DrawFrameSurfaceTest ( void )
{
	FRAME_SURFACE_PTR frameSurface =
		(FRAME_SURFACE_PTR) coreRenderContext[0].rendererContextList.
		rendererContexts[0].renderExit.outputMedia;
	unsigned int *surface = (unsigned int *) frameSurface->backSurface.surface;
	unsigned int width = frameSurface->backSurface.width;
	unsigned int height = frameSurface->backSurface.height;
	int a = coreRenderContext[0].frameStartTime/1000.0f;

	for ( int i = 0; i < height; i ++ ) {
		int r = abs ( (rand () - a)%((height + 1) - rand ()%(i + 1)) );
		for ( int j = 0; j < width; j ++ ) {
			int ca = a*a + 7*a;
			int cb = a*a - 5*a;
			int cc = a*a - 2*a;
			int i1;
			if ( i <= (int)height - r ) {
				i1 = i + r;
			} else {
				i1 = i;
			}
			int j1;
			if ( j <= (int)width - r ) {
				j1 = j + r;
			} else {
				j1 = j;
			}
			int cck = surface[j + i*width]/2 +
				  surface[j + i1*width]/3 +
				  surface[j1 + i*width]/5 +
				  surface[j1 + i1*width]/7;
			FRGB_COLOR fRgb = {(cck+ca)%256/256.0f,
					   (cck+ca)%256/256.0f,
					   (cck+ca)%256/256.0f
					  };
			FRGBToRGB32 ( &fRgb, surface[j1 + i1*width] );
		}// End For

	}// End For

}// End Function DrawFrameSurfaceTest



// Test to draw lines onto the frame surface
void DrawLineToFrameSurfaceTest ( void )
{
	struct point2d p0;
	struct point2d p1;
	FRAME_SURFACE_PTR frameSurface =
		(FRAME_SURFACE_PTR) coreRenderContext[0].rendererContextList.
		rendererContexts[0].renderExit.outputMedia;
	unsigned int width = frameSurface->backSurface.width;
	unsigned int height = frameSurface->backSurface.height;
	int x0 = rand ()%width;
	int y0 = rand ()%height;
	int x1 = rand ()%width;
	int y1 = rand ()%height;
	FRGB_COLOR color;
	InitColor ( (rand ()%255)/255.0f,
		    (rand ()%255)/255.0f,
		    (rand ()%255)/255.0f, &color );

	for ( int i = 0; i < 10000; i ++ ) {
		set_point2d ( x0, y0, &p0 );
		set_point2d ( x1, y1, &p1 );
		x0 = (x0 + x1)%width;
		y0 = (y0 + x0)%height;
		x1 = (x1 + y1)%width;
		y1 = (y1 + y0)%height;
		DrawLineFlat ( &p0, &p1, &color,
			       &frameSurface->backSurface );
	}// End For

}// End Function DrawLineToFrameSurfaceTest



// Test to draw primitives onto the frame surface
void DrawPrimitiveToFrameSurfaceTest ( void )
{
}// End Function DrawPrimitiveToFrameSurfaceTest
#endif
