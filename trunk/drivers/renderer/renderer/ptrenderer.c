/* ptrenderer.c: interface of path tracing renderer go here */
#include <x3d/renderer.h>
#include <x3d/renderable.h>
#include <x3d/renderableaggregaterequest.h>
#include <math/math.h>
#include "lcrenderer.h"
#include "ptrenderer.h"
#include "trianglebuffer.h"
#include "vbo.h"
#include "fbo.h"
#include "colorspectrum.h"

struct rda_context;

enum RenderPassNature {
        RenderPassRadiance,
        RenderPassPostHdr,
        RenderPassFinal
};

struct render_pass {
        enum RenderPassNature   nature;
        int                     src;
        int                     dest;
        float                   dest_blend;
        enum RenderPipeType     pipe;
        int                     sub_pass[10];
        int                     n_subpass;
        struct rda_context*     ctx;
        struct box3d*           simplex;
        enum UtilAccessorType   acc_type;
        struct util_access*     acc_stt;
        struct util_stream      stream[10];
        int                     n_streams;
        struct util_aos         aos_geo;
        struct util_aos         aos_media;
        struct util_image*      src_target;
        struct util_image       target;
};

/** \brief path tracing renderer belongs to the category of
 * single-radiant-pass renderer. So data can be collborated inside here.
 */
struct pt_renderer {
        struct render_bytecode  bytecode;
        struct render_pass      pass[10];
        int                     n_passes;
        struct proj_probe*      probe;
        struct util_image       framebuffer;
        int                     final_pass;
};


struct vertex {
        float   position[3];
        float   normal[3];
        int     mater_ref;
};

struct ray_tree {
        struct vector3d         i_rad;
        struct ray3d            i_ray;
        char                    copy_zone[64];
        void*                   sha_input[12];
        int                     n_emit;
        int                     i_emit;
        struct ray3d            e_ray[2];
};

struct intersect_packet {
        /* writable */
        bool    has_inters;
        int     face[3];
        float   b[3];
        float   bias;
        float   t;
        /* readable */
        struct util_stream*     stream;
        int                     n_streams;
        int*                    i_array;
        struct ray3d*           r;
        /* built-in shader variables */
        void**                  in_uv;
        void**                  in_radiance;
        void**                  out_radiance;
        void**                  in_incident;
        void**                  out_recur_count;
        void**                  out_illum_count;
        void**                  out_recur_ray;
        void**                  out_illum_ray;
        void**                  in_is_vis;
        /* shader stages */
        void (*preprobe) ( void );
        void (*sample) ( void );
        void (*illuminate) ( void );
        void (*transfer) ( void );
        void (*postprobe) ( void );
};

/* intersection callee */
static bool real_subroutine ( int i, struct intersect_packet* ip )
{
        float k;
        int t = i*3;
        struct point3d* p[3];
        p[0] = u_stream_access ( &ip->stream[0], ip->i_array[t + 0] );
        p[1] = u_stream_access ( &ip->stream[0], ip->i_array[t + 1] );
        p[2] = u_stream_access ( &ip->stream[0], ip->i_array[t + 2] );
        if ( intersect_ray_triangle3d ( ip->r, p[0], p[1], p[2],
                                        ip->b, &k, &ip->bias ) &&
             k < ip->t ) {
                ip->face[0] = ip->i_array[t + 0];
                ip->face[1] = ip->i_array[t + 1];
                ip->face[2] = ip->i_array[t + 2];
                ip->t = k;
                ip->has_inters = true;
                return true;
        } else
                return false;
}
static bool simplex_subroutine ( int i, struct box3d* b, struct intersect_packet* ip )
{
        return occlude_line_box3d ( ip->r, b );
}
/* intersection caller */
static bool find_intersect_linear ( struct util_linear* access,
                                    struct intersect_packet* ip )
{
        ip->has_inters = false;
        u_linear_find ( access, ip, simplex_subroutine, real_subroutine );
        return ip->has_inters;
}
static bool is_visible_linear ( struct util_linear* access,
                                struct intersect_packet* ip )
{
        ip->has_inters = false;
        u_linear_find2 ( access, ip, simplex_subroutine, real_subroutine, ip->has_inters );
        return !ip->has_inters;
}


static struct box3d* construct_simplex ( struct util_stream s[], int n_stream,
                                         int* index, int num_index, int* num_simplex )
{
        struct box3d* simplex = alloc_fix ( sizeof(struct box3d), num_index/3 );
        int i;
        for ( i = 0; i < num_index; i += 3 ) {
                struct point3d* curr_vert[3];
                int j;
                for ( j = 0; j < 3; j ++ ) {
                        curr_vert[j] = u_stream_access ( &s[0], index[i + j] );
                }
                struct box3d box;
                init_box3d ( &box );
                int k;
                for ( k = 0; k < 3; k ++ ) {
                        union_box3d_pu ( &box, curr_vert[k] );
                }
                simplex[i/3] = box;
        }
        return simplex;
}

static void free_simplex ( struct box3d* b )
{
        free_fix ( b );
}

static void evaluate_ray_tree ( struct ray_tree* node, struct util_access* acc, struct intersect_packet* ip )
{
        struct ray_tree* root = node;

        while ( true ) {
                if ( node->n_emit == node->i_emit ) {
                        /* backtrack */
                        node --;

                        if ( node == root ) {
                                add_vector3d_u ( &root->i_rad, &(node + 1)->i_rad );
                                node->i_emit ++;

                                if ( root->i_emit == root->n_emit ) {
                                        break;
                                } else {
                                        continue;
                                }
                        } else {
                                u_stream_import_dest_status ( ip->stream, ip->n_streams, node->sha_input );
/*                                int c;
                                for ( c = 0; c < 3; c ++ ) {
                                        g_position[c]   = node->vert_data.position[c];
                                        g_normal[c]     = node->vert_data.normal[c];
                                }
                                g_mater_id = node->vert_data.mater_ref;
                                g_emit = node->i_ray;*/
                                struct vector3d* rad = &(node + 1)->i_rad;
                                struct ray3d* inci = &node->e_ray[node->i_emit];
                                struct vector3d out_rad;

                                *ip->in_radiance = rad;
                                *ip->in_incident = inci;
                                *ip->out_radiance = &out_rad;

                                ip->transfer ( );

                                add_vector3d_u ( &node->i_rad, &out_rad );
                                node->i_emit ++;
                                continue;
                        }
                }
                struct ray3d* r = &node->e_ray[node->i_emit];
                node ++;

                node->i_ray = *r;
                node->i_emit = 0;
                init_vector3d ( &node->i_rad );

                /* trace recursive ray */
                if ( !find_intersect_linear ( (struct util_linear*) acc, ip ) ) {
                        node->n_emit = 0;
                        continue;
                }
/*
                struct vertex* v[3];
                v[0] = &((struct vertex*) tbuf->vertex)[i[0]];
                v[1] = &((struct vertex*) tbuf->vertex)[i[1]];
                v[2] = &((struct vertex*) tbuf->vertex)[i[2]];

                for ( c = 0; c < 3; c ++ ) {
                        node->vert_data.position[c]   = v[0]->position[c]*b[0] +
                                                        v[1]->position[c]*b[1] +
                                                        v[2]->position[c]*b[2];
                        node->vert_data.normal[c]     = v[0]->normal[c]*b[0] +
                                                        v[1]->normal[c]*b[1] +
                                                        v[2]->normal[c]*b[2];
                }
                node->vert_data.mater_ref             = v[0]->mater_ref;

                for ( c = 0; c < 3; c ++ ) {
                        g_position[c]   = node->vert_data.position[c];
                        g_normal[c]     = node->vert_data.normal[c];
                }
                g_mater_id = node->vert_data.mater_ref;
                g_incident = *r;*/
                int n_rray, n_iray;
                struct ray3d rray[10];
                struct ray3d iray[10];
                *ip->in_incident = r;
                *ip->out_illum_count = &n_iray;
                *ip->out_recur_count = &n_rray;
                *ip->out_recur_ray = rray;
                *ip->out_illum_ray = iray;
                u_stream_lerp3_link ( ip->stream, ip->n_streams, ip->face, ip->b );

                ip->sample ( );

                int k;
                for ( k = 0; k < n_rray; k ++ ) {
                        node->e_ray[k] = rray[k];
                }
                node->n_emit = n_rray;

                /* trace illumination ray, and store illumination to current node */
                bool is_vis[1024];
                int j;
                for ( j = 0; j < n_iray; j ++ ) {
                        is_vis[j] = is_visible_linear ( (struct util_linear*) acc, ip );
                }
                *ip->in_is_vis = is_vis;
                *ip->out_radiance = &node->i_rad;

                ip->illuminate ( );

                u_stream_export_dest_status ( ip->stream, ip->n_streams, node->sha_input );
        }
}
#if 0
void test_evaluate ( struct triangle_buffer* tbuf, struct ray_tree* node )
{
        int     i[3];
        float   b[3];
        if ( tbuf_intersect_linear ( tbuf, &node->e_ray[0], i, b ) ) {
                struct vertex* v[3];
                v[0] = &((struct vertex*) tbuf->vertex)[i[0]];
                v[1] = &((struct vertex*) tbuf->vertex)[i[1]];
                v[2] = &((struct vertex*) tbuf->vertex)[i[2]];
                int c;
                for ( c = 0; c < 3; c ++ ) {
                        node->vert_data.position[c]   = v[0]->position[c]*b[0] +
                                                        v[1]->position[c]*b[1] +
                                                        v[2]->position[c]*b[2];
                        node->vert_data.normal[c]     = v[0]->normal[c]*b[0] +
                                                        v[1]->normal[c]*b[1] +
                                                        v[2]->normal[c]*b[2];
                }
                node->vert_data.mater_ref             = v[0]->mater_ref;

                for ( c = 0; c < 3; c ++ ) {
                        g_position[c]   = node->vert_data.position[c];
                        g_normal[c]     = node->vert_data.normal[c];
                }
                g_mater_id = node->vert_data.mater_ref;
//                transfer_shader ();
//                g_incident = *r;
                g_nbounce = 5;
//                mirror_sample_shader ( );

                int k;
                for ( k = 0; k < g_n_recur; k ++ ) {
                        node->e_ray[k] = g_recur[k];
                }
                node->n_emit = g_n_recur;

                /* trace illumination ray */
                int j;
                for ( j = 0; j < g_n_illum; j ++ ) {
                        g_is_vis[j] = tbuf_visible_linear ( tbuf, &g_illum[j] );
                }
//                illuminate_shader ( );

//                int c;
                for ( c = 0; c < 3; c ++ )
                        node->i_rad.c[c] = g_position[c]/800.0f;
//                node->i_rad = g_radiance;
        } else {
                int c;
                for ( c = 0; c < 3; c ++ )
                        node->i_rad.c[c] = 0.0f;
        }
}
#endif // 0
/*
#define rgb_radiance( x )               ((uint8_t) (0XFF) << 24) | \
                                        ((uint8_t)(CLAMP((x)->c[0], 0.0f, 1.0f)*255.0f) << 16) | \
                                        ((uint8_t)(CLAMP((x)->c[1], 0.0f, 1.0f)*255.0f) << 8) | \
                                        ((uint8_t)(CLAMP((x)->c[2], 0.0f, 1.0f)*255.0f) << 0) */
static inline uint32_t rgb_radiance ( struct float_color3* x )
{
        float post_rad[3];
        post_rad[0] = clamp ( powf ( x->rgb[0], 1.0f/2.2f ), 0.0f, 1.0f );
        post_rad[1] = clamp ( powf ( x->rgb[1], 1.0f/2.2f ), 0.0f, 1.0f );
        post_rad[2] = clamp ( powf ( x->rgb[2], 1.0f/2.2f ), 0.0f, 1.0f );

        return ((uint8_t) (0XFF) << 24) | \
               ((uint8_t)(post_rad[0]*255.0f) << 16) | \
               ((uint8_t)(post_rad[1]*255.0f) << 8) | \
               ((uint8_t)(post_rad[2]*255.0f) << 0);
}

static inline double exp_illuminance ( struct float_color3* x )
{
        double illu = 0.3f*x->rgb[0] + 0.6f*x->rgb[1] + 0.1f*x->rgb[2];
        return log ( illu + 1.25 );
}

/* from: http://filmicgames.com/archives/75, Filmic HDR */
static const float A = 0.15f;
static const float B = 0.50f;
static const float C = 0.10f;
static const float D = 0.20f;
static const float E = 0.02f;
static const float F = 0.30f;
static const struct float_color3 W = {11.2f, 11.2f, 11.2f};

static void inv_filimc_curve ( struct float_color3* x, struct float_color3* t )
{
        int i;
        for ( i = 0; i < 3; i ++ )
                t->rgb[i] = 1.0f/(((x->rgb[i]*(A*x->rgb[i] + C*B) + D*E)/
                                   (x->rgb[i]*(A*x->rgb[i] + 1*B) + D*F)) - E/F);
}

static void filmic_curve ( struct float_color3* x, struct float_color3* t )
{
        int i;
        for ( i = 0; i < 3; i ++ )
                t->rgb[i] = ((x->rgb[i]*(A*x->rgb[i] + C*B) + D*E)/
                             (x->rgb[i]*(A*x->rgb[i] + 1*B) + D*F)) - E/F;
}

static inline void rgb_hdr_radiance ( struct float_color3* x, float avg_illum,
                                      struct float_color3* reexp )
{
        const float exp_bias = 5.0f;
        struct float_color3 r;
        int i;
        for ( i = 0; i < 3; i ++ ) {
                r.rgb[i] = x->rgb[i]*exp_bias*avg_illum;
        }

        struct float_color3 c;
        struct float_color3 white_scale;
        filmic_curve ( &r, &c );
        inv_filimc_curve ( (struct float_color3*) &W, &white_scale );

        mul_color3 ( &c, &white_scale, &r );
        *reexp = r;

}

static void render_radiance ( struct pt_radiance_node* render_node )
{
        /* construct intersection packet */
        struct intersect_packet ip;
        int num_index;
        ip.i_array = u_aos_get_index ( &render_node->aos_geo, &num_index );
        ip.n_streams = render_node->n_streams;
        ip.stream = render_node->stream;

        struct ray_tree         s[10] = {0};
        struct ray_tree*        node = s;

        unsigned int width  = render_node->target.width;
        unsigned int height = render_node->target.height;

        struct {
                float dx;
                float dy;
        } spp[4] = {
                [0].dx = 0.0f,
                [0].dy = 0.0f,
                //
                [1].dx = 1.0f/(width*2.0f),
                [1].dy = 0.0f,
                //
                [2].dx = 0.0f,
                [2].dy = 1.0f/(height*2.0f),
                //
                [3].dx = 1.0f/(width*2.0f),
                [3].dy = 1.0f/(height*2.0f)
        };
        const int cNumSamples = 16;
        const int cNumSpp = 4;

        float tw = width - 1;
        float th = height - 1;

        int j;
        for ( j = 0; j < height; j ++ ) {
                printf ( "%d\n", j );
                int i;
                for ( i = 0; i < width; i ++ ) {
                        struct float_color3 acc_rad = {0};
                        int k;
                        for ( k = 0; k < cNumSpp; k ++ ) {
                                struct vector2d uv;
                                uv.x =   2.0f*((float) i/tw + spp[k].dx) - 1.0f;
                                uv.y = -(2.0f*((float) j/th + spp[k].dy) - 1.0f);
                                int n_rray;

                                *ip.in_uv = &uv;
                                *ip.out_recur_count = &n_rray;
                                *ip.out_recur_ray = node->e_ray;

                                int s;
                                for ( s = 0; s < cNumSamples; s ++ ) {
                                        memset ( node, 0, sizeof *node );

                                        ip.preprobe ( );

                                        node->n_emit = n_rray;
                                        //test_evaluate ( &rt->tbuf, node );
                                        evaluate_ray_tree ( node, render_node->acc_stt, &ip );
                                        add_color3 ( &acc_rad, (struct float_color3*) &node->i_rad, &acc_rad );
                                }
                        }
                        struct float_color3* dest = u_image_read ( &render_node->target, 0, i, j );
                        scale_color3 ( 1.0f/((float) (cNumSamples*cNumSpp)), &acc_rad, dest );
                }
        }
}

static void render_hdr ( struct render_pass* pass )
{
        /* hdr postprocessor */
        unsigned int width  = pass->target.width;
        unsigned int height = pass->target.height;

        double exporsure = 0.0;
        int b = 0;
        int j;
        for ( j = 0; j < height; j ++ ) {
                int i;
                for ( i = 0; i < width; i ++ ) {
                        struct float_color3* dest = u_image_read ( pass->src_target, 0, i, j );
                        double k = exp_illuminance ( dest );
                        if ( *(int64_t*)&k != -2251799813685248 )
                                exporsure += k;
                        else b ++;
                }
        }
        exporsure /= (double) height;
        exporsure /= (double) width;
        exporsure = exp ( exporsure );
        for ( j = 0; j < height; j ++ ) {
                int i;
                for ( i = 0; i < width; i ++ ) {
                        struct float_color3* src = u_image_read ( pass->src_target, 0, i, j );
                        struct float_color3* dest = u_image_read ( &pass->target, 0, i, j );
                        rgb_hdr_radiance ( src, exporsure, dest );
                }
        }
}

static void render_output ( struct render_pass* src_pass, struct proj_probe* probe,
                            struct util_image* target )
{
        unsigned int width  = target->width;
        unsigned int height = target->height;
        int j;
        for ( j = 0; j < height; j ++ ) {
                int i;
                for ( i = 0; i < width; i ++ ) {
                        struct float_color3* src = u_image_read ( &src_pass->target, 0, i, j );
                        uint32_t* dest = u_image_read ( target, 0, i, j );
                        *dest = rgb_radiance ( src );
                }
        }
}
/*
void init_ptrenderer ( struct lcrenderer_ops *ops )
{
        ops->lcrenderer_create = cast (ops->lcrenderer_create) create_pt_renderer;
        ops->lcrenderer_free   = cast (ops->lcrenderer_free)   free_pt_renderer;
        ops->lcrenderer_update = cast (ops->lcrenderer_update) pt_renderer_update;
        ops->lcrenderer_render = cast (ops->lcrenderer_render) pt_renderer_render;
        ops->lcrenderer_output = cast (ops->lcrenderer_output) pt_renderer_output;
}

struct pt_renderer *create_pt_renderer ( enum RENDERER_IDR type, struct proj_probe* probe )
{
        struct pt_renderer* t_rend = alloc_fix ( sizeof *t_rend, 1 );
        zero_obj ( t_rend );
        t_rend->probe = probe;
        return t_rend;
}

void free_pt_renderer ( struct pt_renderer* r )
{
        zero_obj ( r );
        free_fix ( r );
}
*/

#if 0

#define get_operand( _op, _obj ) \
{ \
        (_obj) = *(typeof(_obj)*) (_op); \
        (_op) += sizeof(_obj); \
}

void pt_renderer_update ( struct render_bytecode* bytecode, struct pt_renderer* r )
{
        return ;
        r->bytecode = *bytecode;
        /* update passes */
        r->n_passes = 0;
        char* p_instr = r->bytecode.instr;
        while ( true ) {
                enum RENDER_OP op;
                get_operand(p_instr, op);
                switch ( op ) {
                case RENDER_OP_NULL:
                        {
                        goto end_parsing_instr;
                        }
                case RENDER_OP_RADIANCE:
                        {
                        // Removed
                        break;
                        }
                case RENDER_OP_COMPOSITE:
                        {
                        int                     dest;
                        int                     type;
                        int                     src;
                        get_operand ( p_instr, dest );
                        get_operand ( p_instr, type );
                        get_operand ( p_instr, src );

                        struct render_pass* pass = &r->pass[dest];
                        pass->dest = dest;
                        pass->src = src;
                        pass->src_target = &r->pass[src].target;
                        pass->nature = type;
                        r->n_passes ++;
                        break;
                        }
                case RENDER_OP_OUTPUT:
                        {
                        struct proj_probe*      probe;
                        int                     src;
                        get_operand ( p_instr, probe );
                        get_operand ( p_instr, src );

                        r->probe = probe;
                        r->final_pass = src;
                        break;
                        }
                default:
                        {
                        log_mild_err_dbg("op code not handled: %d", op);
                        }
                }
        }
end_parsing_instr:;
        /* generate renderable request */
        int i;
        for ( i = 0; i < r->n_passes; i ++ ) {
                struct render_pass* pass = &r->pass[i];
                pass->sub_pass[RENDERABLE_GEOMETRY] =
                        rda_context_post_request ( pass->ctx, RAG_CULL_NULL,
                                                   nullptr, RENDERABLE_GEOMETRY );
                rda_context_update ( pass->ctx );
        }

        for ( i = 0; i < r->n_passes; i ++ ) {
                struct render_pass* pass = &r->pass[i];
                switch ( pass->nature ) {
                case RenderPassRadiance:
                        {
                        /* update renderable to aos */
                        enum UtilAttribute attri = UtilAttriVertex | UtilAttriNormal;
                        u_aos_free ( &pass->aos_geo );
                        u_aos_init ( &pass->aos_geo, attri );
                        /* - process geometries */
                        int j;
                        int n_rda = rda_context_get_n ( pass->ctx, pass->sub_pass[RENDERABLE_GEOMETRY] );
                        for ( j = 0; j < n_rda; j ++ ) {
                                struct rda_instance* inst = rda_context_get_i ( pass->ctx, j,
                                                                                pass->sub_pass[RENDERABLE_GEOMETRY] );
                                struct rda_geometry* geo  = (struct rda_geometry*) rda_instance_source ( inst );
                                int num_index;
                                int* index = rda_geometry_get_index ( geo, &num_index );
                                int num_vertex;
                                void* vertex = rda_geometry_get_vertex ( geo, &num_vertex );
                                void* normal = rda_geometry_get_normal ( geo, &num_vertex );
                                u_aos_accumulate ( &pass->aos_geo, index, num_index,
                                                    num_vertex, vertex, normal );
                        }
                        /* update shader */
                        void** shader_var_addr[10];

                        /* generate streams */
                        void* vertex[10];
                        int n_vertex = u_aos_get_vertex ( &pass->aos_geo, vertex, &pass->n_streams );
                        bool* avail = u_aos_get_availibility ( &pass->aos_geo );

                        int cSizeOfStream[10] = {};
                        f_Lerp_3 cStreamLerp3[10] = {
                        };
                        int k, m;
                        for ( m = 0, k = 0; k < 10; k ++ ) {
                                if ( avail[k] == true ) {
                                        u_stream_init ( &pass->stream[m],
                                                        vertex[k], cSizeOfStream[k], n_vertex,
                                                        &cSizeOfStream[k], &shader_var_addr[m], 1,
                                                        nullptr, cStreamLerp3[k] );
                                        m ++;
                                }
                        }
                        /* update simplex and accessor utility */
                        struct render_pass* pass = &r->pass[i];
                        free_simplex ( pass->simplex );
                        u_access_free ( pass->acc_stt );

                        int num_index, num_simplex;
                        int* index = u_aos_get_index ( &pass->aos_geo, &num_index );
                        pass->simplex = construct_simplex ( pass->stream, pass->n_streams,
                                                            index, num_index, &num_simplex );
                        pass->acc_stt = u_access_create ( pass->acc_type, pass->simplex, num_simplex );
                        u_access_build ( /* pass->acc_stt */ UtilAccessorLinear );

                        /* create target */
/* @fixme (davis#2#): <pt_renderer_update> use projective probe to determine the render target of a radiance pass */
                        u_image_free ( &pass->target );
                        u_image_init ( &pass->target, 1, UtilImgRGBRadiance, 800, 600 );
                        u_image_alloc ( &pass->target, 0 );
                        }
                case RenderPassPostHdr:
                        {
                        /* create target */
/* @fixme (davis#2#): <pt_renderer_update> use projective probe to determine the render target of a radiance pass */
/*                        u_image_free ( &pass->target );
                        u_image_init ( &pass->target, 1, UtilImgRGBA32, 800, 600 );
                        u_image_alloc ( &pass->target, 0 );*/
                        pass->target = r->pass[pass->src].target;
                        break;
                        }
                case RenderPassFinal:
                        {
                        /* create framebuffer */
/* @fixme (davis#2#): <pt_renderer_update> use projective probe to determine the render target of a radiance pass */
                        u_image_free ( &r->framebuffer );
                        u_image_init ( &r->framebuffer, 1, UtilImgRGBA32, 800, 600 );
                        u_image_alloc ( &r->framebuffer, 0 );
                        break;
                        }
                }
        }
}

void pt_renderer_render ( struct pt_renderer* r )
{
        return ;
        char* p_instr = r->bytecode.instr;
        while ( true ) {
                char op = *p_instr;
                switch ( op ) {
                case RENDER_OP_RADIANCE:
                        {
                        p_instr ++;
                        int                     dest;
                        struct rda_context*     ctx;
                        enum RenderPipeType    pipeline;
                        float                   blend;
                        get_operand ( p_instr, dest );
                        get_operand ( p_instr, ctx );
                        get_operand ( p_instr, pipeline );
                        get_operand ( p_instr, blend );

                        /* construct intersect packet */
                        struct render_pass* pass = &r->pass[dest];
                        render_radiance ( pass );
                        break;
                        }
                case RENDER_OP_COMPOSITE:
                        {
                        p_instr ++;
                        int                     dest;
                        int                     type;
                        int                     src;
                        get_operand ( p_instr, dest );
                        get_operand ( p_instr, type );
                        get_operand ( p_instr, src );

                        struct render_pass* pass = &r->pass[dest];
                        render_hdr ( pass );
                        break;
                        }
                case RENDER_OP_OUTPUT:
                        {
                        p_instr ++;
                        struct proj_probe*      probe;
                        int                     src;
                        get_operand ( p_instr, probe );
                        get_operand ( p_instr, src );

                        struct render_pass* src_pass = &r->pass[src];
                        render_output ( src_pass, r->probe, &r->framebuffer );
                        break;
                        }
                }
        }
}
#endif // 0
void pt_renderer_output ( struct pt_renderer* r )
{
}

struct render_node_ex_impl* pt_radiance_node_creator(struct render_node_ex* parent)
{
        struct pt_radiance_node* node = alloc_obj(node);
        zero_obj(node);
        node->_parent2 = *(struct render_radiance*) parent;
        // fill in ops
        struct render_node_ex_ops       ops;
        ops.f_compute           = pt_radiance_node_compute;
        ops.f_free              = pt_radiance_node_free;
        ops.f_get_result        = pt_radiance_node_get_result;
        ops.f_is_compatible     = pt_radiance_node_is_compatible;
        node->_parent.ops       = ops;
        return (struct render_node_ex_impl*) node;
}

bool pt_radiance_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        /* It will be compatible if the radiance node is:
         * 1. software based
         * 2. preferred path tracing renderer
         * 3. using solid geometry model
         * 4. with direct lighting rendering pipeline */
        enum RenderSpecType* spec = render_tree_retrieve_environment(tree, RenderEnvSpec);
        if (*spec != RenderSpecSWBuiltin) {
                return false;
        }
        enum PreferredRendererType* renderer = render_tree_retrieve_environment(tree, RenderEnvPreferredRenderer);
        if (*renderer != RendererPathTracer) {
                return false;
        }
        enum GeometryModelType* model = render_tree_retrieve_environment(tree, RenderEnvGeometryModel);
        if (*model != GeometryModelSolid) {
                return false;
        }
        enum RenderPipeType pipe = render_node_radiance_get_pipe(&((struct pt_radiance_node*) self)->_parent2);
        if (pipe != RenderPipeDirectLighting) {
                return false;
        }
        return true;
}

void pt_radiance_node_compute(struct render_node_ex_impl* self,
                              const struct render_node_ex_impl* input[],
                              const struct render_node_ex_impl* output[])
{
        struct pt_radiance_node* node = (struct pt_radiance_node*) self;
        // Get input node instance <renderable loader> and its context
        const int slot                                  = render_node_radiance_get_input_slot(RenderNodeRenderableLoader);
        struct pt_renderable_loader_node* rdaloader     = (struct pt_renderable_loader_node*) input[slot];
        struct rda_context* context                     = rdaloader->_parent.ops.f_get_result(&rdaloader->_parent);
        // Get geometry data
        uuid_t request_id                               = rda_context_post_request(context, RAG_CULL_NULL, nullptr,
                                                                                   RENDERABLE_GEOMETRY);
        const int n                                     = rda_context_get_n(context, request_id);
        u_aos_free(&node->aos_geo);
        u_aos_init(&node->aos_geo, UtilAttriVertex | UtilAttriNormal);
        int i;
        for (i = 0; i < n; i ++) {
                struct rda_instance* inst = rda_context_get_i(context, i, request_id);
                struct rda_geometry* geometry = (struct rda_geometry*) rda_instance_source(inst);
                int num_index;
                int* index      = rda_geometry_get_index(geometry, &num_index);
                int num_vertex;
                void* vertex    = rda_geometry_get_vertex(geometry, &num_vertex);
                void* normal    = rda_geometry_get_normal(geometry, &num_vertex);
                u_aos_accumulate(&node->aos_geo, index, num_index, num_vertex, vertex, normal);
        }
        void** shader_var_addr[10];

        /* generate streams */
        void* vertex[10];
        int n_vertex = u_aos_get_vertex(&node->aos_geo, vertex, &node->n_streams);
        bool* avail = u_aos_get_availibility(&node->aos_geo);

        int cSizeOfStream[10] = {};
        f_Lerp_3 cStreamLerp3[10] = {
        };
        int k, m;
        for (m = 0, k = 0; k < 10; k ++)
        {
                if (avail[k] == true) {
                        u_stream_init(&node->stream[m],
                                      vertex[k], cSizeOfStream[k], n_vertex,
                                      &cSizeOfStream[k], &shader_var_addr[m], 1, nullptr, cStreamLerp3[k]);
                        m ++;
                }
        }
        /* update simplex and accessor utility */
        free_simplex(node->simplex);
        u_access_free(node->acc_stt);

        int num_index, num_simplex;
        int* index = u_aos_get_index(&node->aos_geo, &num_index);
        node->simplex = construct_simplex(node->stream, node->n_streams,
                                          index, num_index, &num_simplex);
        /* @fixme (davis#9#): <pt_radiance_node_compute> hard coded acc_type */
        node->acc_type = UtilAccessorLinear;
        node->acc_stt = u_access_create(node->acc_type, node->simplex, num_simplex);
        u_access_build(node->acc_stt);

        /* create target */
        /* @fixme (davis#2#): <pt_renderer_update> use projective probe to determine the render target of a radiance pass */
        u_image_free(&node->target);
        u_image_init(&node->target, 1, UtilImgRGBRadiance, 800, 600);
        u_image_alloc(&node->target, 0);

        render_radiance(node);
}

void* pt_radiance_node_get_result(struct render_node_ex_impl* self)
{
        return &((struct pt_radiance_node*) self)->target;
}

void pt_radiance_node_free(struct render_node_ex_impl* self)
{
}

