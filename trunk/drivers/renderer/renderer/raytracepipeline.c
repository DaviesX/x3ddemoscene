/* raytracepipeline.c: All ray tracing based pipelines go here. Each pipeline decides the method of integration of ray samples to generate the final result */
#include <container/stack.h>
#include <system/allocator.h>
#include <system/log.h>
#include <math/math.h>
#include <x3d/renderer.h>
#include "colorspectrum.h"
#include "vbo.h"
#include "fbo.h"
#include "trianglebuffer.h"
#include "shaderformulate.h"
#include "raytracepipeline.h"


struct pathtrace_pipeline {
        enum RENDER_SPEC_IDR    spec;
        int                     nthreads;
        int                     nsamples;
        int                     nprobes;
        void*                   probe_buf;
        struct shader_attribute probe_attr;
        struct triangle_buffer  tribuf;
        struct shader_attribute vbo_attr;
        struct shader_form      emitter;
        struct shader_form      bounce;
        struct shader_form      transfer;
};

struct pathtrace_pipeline* create_pathtrace_pipeline ( void )
{
        return nullptr;
}

void free_pathtrace_pipeline ( struct pathtrace_pipeline* pipeline )
{
}

static void rda_to_tribuf ( struct renderable* rda, int n,
                            struct triangle_buffer* buf )
{
        int i;
        for ( i = 0; i < n; i ++ ) {
        }
}

static void pathtrace_radiance_cpu ( struct pathtrace_pipeline* pipeline,
                                     struct fbo* target,
                                     struct renderable* rda, int n );
static void pathtrace_radiace_gpgpu ( struct pathtrace_pipeline* pipeline,
                                      struct fbo* target,
                                      struct renderable* rda, int n );

void pathtrace_radiance ( struct pathtrace_pipeline* p,
                          struct fbo* target,
                          struct renderable* rda, int n )
{
        switch ( p->spec ) {
        case RENDER_SPEC_SW_BUILTIN:
                pathtrace_radiance_cpu ( p, target, rda, n );
                break;
        case RENDER_SPEC_HW_OPENGL:
                pathtrace_radiace_gpgpu ( p, target, rda, n );
                break;
        default:
                log_severe_err_dbg ( "unknown hardware specification" );
                break;
        }
}

static bool access_box ( void* data, struct box3d* bound )
{
        return false;
}

static bool access_triangle ( void* data, struct triangle_iter* iter )
{
        return false;
}

struct transfer_stream {
        int32_t io_buffer[16];
};

struct hit_face {
        struct vector3d normal;
        int             mater;
};

struct tmp_material {
        float diff_r, diff_g, diff_b;
        float spec_r, spec_g, spec_b;
};

enum {
        MATER_RED,
        MATER_GREEN,
        MATER_BLUE,
        MATER_WHITE
};

const struct tmp_material cMaterial = {
};

static uint32_t brdf ( struct vector3d* normal, int mat )
{
        return 0;
}

static void pathtrace_radiance_cpu ( struct pathtrace_pipeline* p,
                                     struct fbo* target,
                                     struct renderable* rda, int n )
{
        struct triangle_buffer  buf;
        struct vbo*             vbo;
        tri_buf_init ( &buf );
        rda_to_tribuf ( rda, n, &buf );
        tri_buf_generate_accessor ( &buf, TRIBUFFER_ACC_LINEAR );
        tri_buf_generate_vbo ( &buf );
        vbo = tri_buf_get_vbo ( &buf );

        int w, h, s;
        untyped* fbo_dest;
        fbo_dimension ( target, &w, &h, &s );
        fbo_dest = fbo_memory ( target );
        printf ( "%x", (unsigned int) fbo_dest );
//        declare_stack ( stack, sizeof(struct transfer_stream)*10 );

        struct intersect {
                struct line3d ray;
                float b0, b1, b2;
        } p_hit;

        int x, y;
        for ( y = 0; y < h; y ++ ) {
                for ( x = 0; x < w; x ++ ) {
                        float u = 2.0f*((float) x/w) - ((float) x/w);
                        float v = 2.0f*((float) y/h) - ((float) y/h);
                        struct point3d o = {0.0f, 0.0f, 0.0f};
                        struct point3d p = {u,    v,    1.0f};
                        struct line3d ray;
                        build_line3d_t ( &o, &p, 1.0f, FLOAT_INFINITE, &ray );
                        /* bounce */
                        struct triangle_iter iter;
                        tri_buf_access ( &buf, &p_hit, &iter,
                                         access_box, access_triangle );
                        struct hit_face* attri[3];
                        int i;
                        for ( i = 0; i < 3; i ++ ) {
                                int j = tri_iter_i ( &iter, i );
                                attri[i] = vbo_i_x86 ( vbo, 0, j );
                        }
                        struct vector3d n;
                        n.x = attri[0]->normal.x*p_hit.b0 +
                              attri[1]->normal.x*p_hit.b1 +
                              attri[2]->normal.x*p_hit.b2;
                        n.y = attri[0]->normal.y*p_hit.b0 +
                              attri[1]->normal.y*p_hit.b1 +
                              attri[2]->normal.y*p_hit.b2;
                        n.z = attri[0]->normal.z*p_hit.b0 +
                              attri[1]->normal.z*p_hit.b1 +
                              attri[2]->normal.z*p_hit.b2;
                        printf ( "%f", n.x );
                }
        }
}

static void pathtrace_radiance_cpu2 ( struct pathtrace_pipeline* p,
                                      struct fbo* target,
                                      struct renderable* rda, int n )
{
        struct triangle_buffer  buf;
        struct vbo*             vbo;
        tri_buf_init ( &buf );
        rda_to_tribuf ( rda, n, &buf );
        tri_buf_generate_accessor ( &buf, TRIBUFFER_ACC_LINEAR );
        tri_buf_generate_vbo ( &buf );
        vbo = tri_buf_get_vbo ( &buf );

        int w, h, s;
        untyped* fbo_dest;
        fbo_dimension ( target, &w, &h, &s );
        fbo_dest = fbo_memory ( target );

        declare_stack ( stack, sizeof(struct transfer_stream)*10 );

        f_Emitter emitter_shader   = (f_Emitter) shaform_exe_x86 ( &p->emitter );
        f_Bounce bounce_shader     = (f_Bounce) shaform_exe_x86 ( &p->bounce );
        f_Transfer transfer_shader = (f_Transfer) shaform_exe_x86 ( &p->transfer );

        struct transfer_stream* first = stack_ptr(&stack);
        shaform_output_buffer_x86 ( &p->emitter, first->io_buffer );

        struct {
                struct line3d ray;
                float b0, b1, b2;
        } intersect;
//        shaform_output_addr_x86 ( &p->emitter, SHADER_OUTPUT_RAY, &intersect.ray );

        int i;
        for ( i = 0; i < p->nprobes; i ++ ) {
                /*                void* probe_in = (untyped*) p->probe_buf + i*attr_size ( &p->probe_attr );
                                shaform_input_buffer_x86 ( &p->emitter, 0, probe_in );*/
                int j, k;
                for ( k = 0; k < h; k ++ ) {
                        for ( j = 0; j < w; j ++ ) {
                                int l;
                                for ( l = 0; l < p->nsamples; l ++ ) {
                                        struct vector2d uv = {
                                                (float) j/(w - 1),
                                                (float) k/(h - 1)
                                        };
                                        shaform_input_addr_x86 ( &p->emitter, SHADER_INPUT_UV, &uv );
                                        emitter_shader ();
                                        do {
                                                struct triangle_iter iter;
                                                tri_buf_access ( &buf, &intersect, &iter,
                                                                 access_box, access_triangle );
                                                void* vattri[3];
                                                untyped dattri[10][64];
                                                int s;
                                                for ( s = 0; s < vbo_n_stream ( vbo ); s ++ ) {
                                                        int v;
                                                        for ( v = 0; v < 3; v ++ ) {
                                                                int hit_i = tri_iter_i ( &iter, 0 );
                                                                vattri[v] = vbo_i_x86 ( vbo, s, hit_i );
                                                        }
                                                        attr_op_trilerp ( &p->bounce.attri[s],
                                                                          vattri[0], vattri[1], vattri[2],
                                                                          intersect.b0, intersect.b1, intersect.b2,
                                                                          &dattri[s] );
                                                        shaform_input_buffer_x86 ( &p->bounce, s, &dattri[s] );
                                                }

                                                stack_push_ptr ( &stack, first );
                                                struct transfer_stream* curr = stack_ptr(&stack);
                                                shaform_output_buffer_x86 ( &p->bounce, curr->io_buffer );
                                        } while ( bounce_shader () );

                                        void* radiance = fbo_dest + (k*w + j)*s;
                                        shaform_output_addr_x86 ( &p->transfer, SHADER_OUTPUT_RADIANCE, radiance );
                                        do {
                                                struct transfer_stream* curr = stack_ptr(&stack);
                                                shaform_output_buffer_x86 ( &p->bounce, curr->io_buffer );
                                                transfer_shader ();
                                                stack_pop_ptr ( &stack, curr );
                                        } while ( !stack_is_empty ( &stack ) );
                                }
                        }
                }
        }
}

static void pathtrace_radiace_gpgpu ( struct pathtrace_pipeline* p,
                                      struct fbo* target,
                                      struct renderable* rda, int n )
{
}

