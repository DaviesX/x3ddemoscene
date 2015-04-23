/* main.c: Unit tests to all rendering functions go here */
#include <x3d/common.h>
#include <container/paramset.h>
#include <system/log.h>
#include <x3d/debug.h>
#include <math/math.h>
#include "main.h"


/* utility functions */
static void print_point4d ( struct point4d *p )
{
        log_normal ( "p(%f, %f, %f, %f)", p->x, p->y, p->z, p->w );
}

static void print_line4d ( struct point4d *p0, struct point4d *p1 )
{
        log_normal ( "p0(%f, %f, %f, %f) --> p1(%f, %f, %f, %f)",
                     p0->x, p0->y, p0->z, p0->w,
                     p1->x, p1->y, p1->z, p1->w );
}

static void print_triangle4d ( struct point4d *p0, struct point4d *p1, struct point4d *p2 )
{
        log_normal ( "T:\n\tV0(%f, %f, %f, %f)\n\tV1(%f, %f, %f, %f)\n\tV2(%f, %f, %f, %f)",
                     p0->x, p0->y, p0->z, p0->w,
                     p1->x, p1->y, p1->z, p1->w,
                     p2->x, p2->y, p2->z, p2->w );
}

/* vertex prcessor's */		#include "vertexprocessor.h"
#include "dbg_vertex_processor.h"
#include "rasterization.h"

static void vert_post_process ( struct alg_named_params *global_params );

static void vert_post_process ( struct alg_named_params *global_params )
{
        struct dbg_vertex {
                struct point4d v;
                int p;
        };
        struct dbg_vertex v[3];
        struct dbg_vertex vo[3];
        // struct dbg_vertex tmp_cache[20];
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
                n = dbg_process_point (
                            (rt_vertex *) &v[i], &t_all, sizeof v[0], (rt_vertex *) &vo[i] );
                if ( n > 0 ) {
                        print_point4d ( &vo[i].v );
                } else {
                        log_normal_dbg ( "p%d: out", i );
                }
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
        n = dbg_process_line (
                    (rt_vertex *) v, &t_all,
                    comp_offset, comp_format, 2, sizeof v[0],
                    (rt_vertex *) vo );
        if ( n > 0 ) {
                print_line4d ( &vo[0].v, &vo[1].v );
        } else {
                log_normal_dbg ( "line out" );
        }
        /* triangle test */
        set_point4d ( 100.0f, -10.0f, 80.0f, 1.0f, &v[0].v );
        v[0].p = 0;
        set_point4d ( 100.0f, 40.0f, 120.0f, 1.0f, &v[1].v );
        v[1].p = 1;
        set_point4d ( 150.0f, 0.0f, 120.0f, 1.0f, &v[2].v );
        v[2].p = 2;
        n = dbg_process_triangle (
                    (rt_vertex *) v, &t_view, &t_all, 1.0f,
                    comp_offset, comp_format, 2, sizeof v[0],
                    (rt_vertex *) vo );
        if ( n > 0 ) {
                print_triangle4d ( &vo[0].v, &vo[1].v, &vo[2].v );
        } else {
                log_normal_dbg ( "triangle out" );
        }
}

/* rasterizer's */		#include "rasterizer.h"
static void dbg_rasterizer_add_all ( void )
{
}

/* rtcontext's */
#include "rasterization.h"
#include <math/math.h>
#include "vibuffer.h"
#include "colorspectrum.h"
#include "surface.h"
#include <renderer/shader.h>
#include <renderer/out.h>


static void simple_rt_pipeline_init ( struct alg_named_params *global_params );
static void simple_rt_pipeline ( struct alg_named_params *global_params );
static void simple_rt_pipeline_free ( struct alg_named_params *global_params );

static struct rtcontext g_rtctx;
static struct matrix4x4 g_view, g_proj;
static struct surface *g_color_surf;
static struct surface *g_depth_surf;
static struct vertex_buffer *g_vb;
static struct index_buffer *g_ib;
static struct shader *g_vs;
static struct shader *g_fs;
static struct render_out g_ro;

static void make_cube_vibuffer ( float x, float y, float z, float s,
                                 struct vertex_buffer *vb, struct index_buffer *ib )
{
        struct point3d v[8] = {
                [0].p = { 1.0*s + x,  1.0*s + y, -1.0*s + z},
                [1].p = { 1.0*s + x, -1.0*s + y, -1.0*s + z},
                [2].p = { 1.0*s + x, -1.0*s + y,  1.0*s + z},
                [3].p = { 1.0*s + x,  1.0*s + y,  1.0*s + z},
                [4].p = {-1.0*s + x,  1.0*s + y,  1.0*s + z},
                [5].p = {-1.0*s + x, -1.0*s + y,  1.0*s + z},
                [6].p = {-1.0*s + x, -1.0*s + y, -1.0*s + z},
                [7].p = {-1.0*s + x,  1.0*s + y, -1.0*s + z},
        };
        int i;
        for ( i = 0; i < 8; i ++ ) {
                vbuffer_add_vertex ( vb );
                vibuffer_add_element ( &v[i], vb );
        }
        int index[12*3] = {
                0, 2, 1,
                0, 3, 2,
                3, 5, 2,
                3, 4, 5,
                4, 6, 5,
                4, 7, 6,
                7, 1, 6,
                7, 0, 1,
                7, 3, 0,
                7, 4, 3,
                6, 1, 2,
                6, 2, 5
        };
        for ( i = 0; i < 12*3; i ++ ) {
                ibuffer_add_index ( ib );
                vibuffer_add_element ( &index[i], ib );
        }
}

static void simple_rt_pipeline_init ( struct alg_named_params *global_params )
{
        init_rtcontext ( &g_rtctx );

        /* a volume with fov = 90 degree, w/h = 4/3, n = 1.0, f = 100.0 */
        identity_matrix4x4 ( &g_view );
        set_matrix4x4 ( &g_proj,
                        1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0*4.0/3.0, 0.0, 0.0,
                        0.0, 0.0, (100.0 + 1.0)/(100.0 - 1.0), 1.0,
                        0.0, 0.0, 2.0*100.0*1.0/(1.0 - 100.0), 0.0 );
        rtcontext_set_view ( &g_view, &g_rtctx );
        rtcontext_set_proj ( &g_proj, &g_rtctx );

        rtcontext_set_buffer ( RT_COLOR_BUFFER,
                               RT_BUFFER_ENABLE | RT_BUFFER_WRITE, &g_rtctx );
        rtcontext_set_buffer ( RT_DEPTH_BUFFER,
                               RT_BUFFER_ENABLE | RT_BUFFER_WRITE, &g_rtctx );
        rtcontext_set_buffer ( RT_VERTEX_BUFFER,
                               RT_BUFFER_ENABLE | RT_BUFFER_WRITE, &g_rtctx );
        rtcontext_set_buffer ( RT_INDEX_BUFFER,
                               RT_BUFFER_ENABLE | RT_BUFFER_WRITE, &g_rtctx );

        g_color_surf = create_surface ( 800, 600, SF_IDR_IA8R8G8B8 );
        struct float_color4 c;
        c.r = 1.0f;
        c.g = 1.0f;
        c.b = 1.0f;
        c.a = 0.0f;
        surface_fill_color ( &c, g_color_surf );
        rtcontext_bind_color_buffer ( g_color_surf, &g_rtctx );
        g_depth_surf = create_surface ( 800, 600, SF_IDR_F32 );
        rtcontext_bind_depth_buffer ( g_depth_surf, &g_rtctx );
        rtcontext_set_depth_func ( RT_EQUALITY_TRUE, &g_rtctx );

        g_vb = create_vbuffer ( false );
        g_ib = create_ibuffer ( false );
        struct vertattri vattri;
        vertattri_init ( &vattri );
        vertattri_set ( 0, VERTEX_ELM_FLOAT, 3, &vattri );
        vbuffer_bind_vertattri ( &vattri, g_vb );
        ibuffer_set_format ( INDEX_ELEMENT_INT32, g_ib );
        make_cube_vibuffer ( -42.0, 0.0, 50.0, 5.0f, g_vb, g_ib );
        rtcontext_bind_vertex_buffer ( g_vb, &g_rtctx );
        rtcontext_bind_index_buffer ( g_ib, &g_rtctx );

        struct vertdefn vdefn;
        vertdefn_init ( &vdefn );
        vertdefn_set ( 0, VERTEX_DEFN_FLOAT4 | VERTEX_DEFN_INTERPOLATE, &vdefn );
        vertdefn_fix ( &vdefn );
        rtcontext_bind_vertdefn ( &vdefn, &g_rtctx );

        g_vs = create_shader ( SHADER_STAGE_VERTEX, false, SHADER_SPEC_BUILTIN );
        g_fs = create_shader ( SHADER_STAGE_FRAGMENT, false, SHADER_SPEC_BUILTIN );
        shader_add_function ( "vs_to_modelview_a", g_vs );
        shader_add_function ( "fs_pure_color_a", g_fs );
        shader_finalize ( g_vs );
        shader_finalize ( g_fs );
        struct matrix4x4 t_world;
        identity_matrix4x4 ( &t_world );
        shader_set_uniform ( SHADER_UNI_MODELVIEW, &t_world,
                             sizeof t_world, g_fs );
        rtcontext_bind_shader ( g_vs, RT_VERTEX_SHADER, &g_rtctx );
        rtcontext_bind_shader ( g_fs, RT_FRAGMENT_SHADER, &g_rtctx );

        rtcontext_draw_mode ( RT_WIREFRAME_MODE, &g_rtctx );
        rtcontext_set_primitive_type ( TRIANGLE_PRIMITIVE, &g_rtctx );
        rtcontext_set_spec ( RT_BUILTIN_SW, &g_rtctx );
        rtcontext_cull ( CULL_IDR_BACKFACE, &g_rtctx );
        rtcontext_finalize_pipeline ( &g_rtctx );

        init_render_out ( OUT_IDR_SCREEN, OUT_GTK_IMPL, &g_ro );
        void *widget = nullptr;
//        void *widget = dbg_get_render_region ();
        ro_create_screen ( widget, 0, 0, 800, 600, OUT_SCREEN_ARGB32, &g_ro );
}

static void simple_rt_pipeline_free ( struct alg_named_params *global_params )
{
}

static void simple_rt_pipeline ( struct alg_named_params *global_params )
{
        rasterization_run_pipeline ( &g_rtctx, false );
//        render_out_retrieve ( surface_get_addr ( g_color_surf ), 4, &g_ro );
//        render_out_run ( &g_ro );
//        dbg_hand_image_to_display ( surface_get_addr ( g_color_surf ), 800, 600 );
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
