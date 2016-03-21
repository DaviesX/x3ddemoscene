/* ptrenderer.c: interface of path tracing renderer go here */
#include <math/math.h>
#include <system/symlib.h>
#include <system/thread.h>
#include <container/paramset.h>
#include <x3d/renderer.h>
#include <x3d/renderable.h>
#include <x3d/renderableaggregaterequest.h>
#include <x3d/projectionprobe.h>
#include <x3d/debug.h>
#include <renderer/shader.h>
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
        uint8_t                 buffer[64];
        int                     n_emit;
        int                     i_emit;
        struct ray3d            e_ray[4];
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

// lerp utility functions
static void lerp_point(struct point3d* p[3], float t[3], struct point3d* xo)
{
        int i;
        for (i = 0; i < 3; i ++) {
                xo->p[i] = p[0]->p[i]*t[0] + p[1]->p[i]*t[1] + p[2]->p[i]*t[2];
        }
}

static void lerp_matid(int* p[3], float t[3], int* xo)
{
        *xo = *p[0];
}

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
        ip->has_inters  = false;
        ip->t           = FLOAT_MAX;
        u_linear_find ( access, ip, simplex_subroutine, real_subroutine );
        return ip->has_inters;
}
static bool is_visible_linear ( struct util_linear* access,
                                struct intersect_packet* ip )
{
        ip->has_inters  = false;
        ip->t           = FLOAT_MAX;
        u_linear_find2 ( access, ip, simplex_subroutine, real_subroutine, ip->has_inters );
        return !ip->has_inters;
}


static struct box3d* construct_simplex ( struct util_stream s[], int n_stream,
                                         int* index, int num_index, int* num_simplex )
{
        *num_simplex = num_index/3;     // just the number of triangles
        struct box3d* simplex = alloc_fix(sizeof(struct box3d), num_index/3);
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
                if (node->n_emit == node->i_emit) {
                        /* backtrack */
                        node --;

                        if (node == root) {
                                add_vector3d_u ( &root->i_rad, &(node + 1)->i_rad );
                                node->i_emit ++;

                                if ( root->i_emit == root->n_emit ) {
                                        break;
                                } else {
                                        continue;
                                }
                        } else {
                                u_stream_load_from_buffer(ip->stream, ip->n_streams, node->buffer);
                                struct vector3d rad             = node[1].i_rad;
                                struct ray3d* inci              = &node->e_ray[node->i_emit];
                                *ip->in_radiance                = &rad;
                                *ip->in_incident                = inci;
                                *ip->out_radiance               = &rad;

                                ip->transfer();

                                add_vector3d_u ( &node->i_rad, &rad );
                                node->i_emit ++;
                                continue;
                        }
                }
                ip->r = &node->e_ray[node->i_emit];
                node ++;

                node->i_ray = *ip->r;
                node->i_emit = 0;
                init_vector3d ( &node->i_rad );

                /* trace recursive ray */
                if ( !find_intersect_linear ( (struct util_linear*) acc, ip )) {
                        node->n_emit = 0;
                        continue;
                }
                int n_rray, n_iray;
                struct ray3d iray[10];
                *ip->in_incident = ip->r;
                *ip->out_illum_count = &n_iray;
                *ip->out_recur_count = &n_rray;
                *ip->out_recur_ray = node->e_ray;
                *ip->out_illum_ray = iray;
                u_stream_lerp3_link(ip->stream, ip->n_streams, ip->face, ip->b);
                u_stream_store_to_buffer(ip->stream, ip->n_streams, node->buffer);

                ip->sample();

                node->n_emit = n_rray;

                /* trace illumination ray, and store illumination to current node */
                bool is_vis[1024];
                int j;
                for ( j = 0; j < n_iray; j ++ ) {
                        ip->r = &iray[j];
                        is_vis[j] = is_visible_linear ( (struct util_linear*) acc, ip );
                }
                *ip->in_is_vis = is_vis;
                *ip->out_radiance = &node->i_rad;

                ip->illuminate();
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

#include "./shader/ptshader.c"

static void render_radiance(struct pt_radiance_node* render_node, struct util_image* target)
{
        /* construct intersection packet */
        struct intersect_packet ip;
        int num_index;
        ip.i_array = u_aos_get_index(&render_node->aos_geo, &num_index);
        ip.n_streams = render_node->n_streams;
        ip.stream = render_node->stream;
/**@todo (davis#1#) <intersect_packet> these should be done through shader formulate context */
        ip.in_uv                = (void**) &g_uv;
        ip.in_radiance          = (void**) &g_radiance;
        ip.in_incident          = (void**) &g_incident;
        ip.in_is_vis            = (void**) &g_is_vis;
        ip.out_illum_count      = (void**) &g_n_illum;
        ip.out_illum_ray        = (void**) &g_illum;
        ip.out_radiance         = (void**) &g_radiance;
        ip.out_recur_count      = (void**) &g_n_recur;
        ip.out_recur_ray        = (void**) &g_recur;
        ip.preprobe             = a_easyrt_probe;
        ip.sample               = a_easyrt_sample;
        ip.illuminate           = a_easyrt_illuminate;
        ip.transfer             = a_easyrt_transfer;
        ip.postprobe            = nullptr;
        /* gives temporary buffering */
        ray3 illum_rays[1024];
        *ip.out_illum_ray = illum_rays;

        struct ray_tree         s[10] = {0};
        struct ray_tree*        node = s;

        unsigned int width  = target->width;
        unsigned int height = target->height;

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
        const int cNumSpp = 1;

        float tw = width - 1;
        float th = height - 1;

        int j;
        for (j = 0; j < height; j ++) {
                // printf ( "%d\n", j );
                int i;
                for (i = 0; i < width; i ++) {
                        struct vector3d acc_rad = {0};
                        int k;
                        for (k = 0; k < cNumSpp; k ++) {
                                struct vector2d uv;
                                uv.x =   2.0f*((float) i/tw + spp[k].dx) - 1.0f;
                                uv.y = -(2.0f*((float) j/th + spp[k].dy) - 1.0f);

                                int s;
                                for ( s = 0; s < cNumSamples; s ++ ) {
                                        memset(node, 0, sizeof *node);
                                        int n_rray;     // buffer for the shader
                                        *ip.in_uv = uv.p;
                                        *ip.out_recur_count = &n_rray;
                                        *ip.out_recur_ray = node->e_ray;

                                        ip.preprobe();

                                        node->n_emit = n_rray;
                                        //test_evaluate ( &rt->tbuf, node );
                                        evaluate_ray_tree(node, render_node->acc_stt, &ip);
                                        add_vector3d(&acc_rad, &node->i_rad, &acc_rad);
                                }
                        }
                        struct vector3d* dest = u_image_read(target, 0, i, j);
                        scale_vector3d(1.0f/((float) (cNumSamples*cNumSpp)), &acc_rad, dest);
                }
        }
}

static void render_hdr(struct render_pass* pass)
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

static void render_output(struct util_image* src_target, struct util_image* target)
{
        unsigned int width  = target->width;
        unsigned int height = target->height;
        int j;
        for (j = 0; j < height; j ++) {
                int i;
                for (i = 0; i < width; i ++) {
                        struct float_color3* src        = u_image_read(src_target, 0, i, j);
                        uint32_t* dest                  = u_image_read(target, 0, i, j);
                        *dest = rgb_radiance(src);
                }
        }
}

void pt_renderer_output ( struct pt_renderer* r )
{
}


void pt_renderer_system_init(struct symbol_set* symbols)
{
        init_shader_library();
//        shader_var_addr[_AttriVertex] = symlib_ret_variable(symbols, "g_position", nullptr);
//        shader_var_addr[_AttriNormal] = symlib_ret_variable(symbols, "g_normal", nullptr);
}

// radiance node
struct render_node_ex_impl* pt_radiance_node_creator(struct render_node_ex* parent)
{
        struct pt_radiance_node* node = alloc_obj(node);
        zero_obj(node);
        // node->_parent2 = *(struct render_radiance*) parent;
        // fill in ops for the parent
        struct render_node_ex_ops       ops;
        ops.f_compute           = pt_radiance_node_compute;
        ops.f_free              = pt_radiance_node_free;
        ops.f_get_result        = pt_radiance_node_get_result;
        ops.f_is_compatible     = pt_radiance_node_is_compatible;
        node->_parent.ops       = ops;
        return (struct render_node_ex_impl*) node;
}

char* pt_radiance_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        /* It will be compatible if the radiance node is:
         * 1. software based
         * 2. preferred path tracing renderer
         * 3. using solid geometry model
         * 4. with direct lighting rendering pipeline */
        enum RenderSpecType* spec = render_tree_retrieve_environment(tree, nullptr, RenderEnvSpec);
        if (spec && *spec != RenderSpecSWBuiltin) {
                return "pt_renderer: spec && *spec != RenderSpecSWBuiltin";
        }
        enum PreferredRendererType* renderer = render_tree_retrieve_environment(tree, nullptr, RenderEnvPreferredRenderer);
        if (renderer && *renderer != RendererPathTracer) {
                return "pt_renderer: renderer && *renderer != RendererPathTracer";
        }
        enum GeometryModelType* model = render_tree_retrieve_environment(tree, nullptr, RenderEnvGeometryModel);
        if (model && *model != GeometryModelSolid) {
                return "pt_renderer: model && *model != GeometryModelSolid";
        }
        struct render_radiance* parent = cast(parent) render_node_ex_impl_get_ex(self);
        enum RenderPipeType pipe = render_node_radiance_get_pipe(parent);
        if (pipe != RenderPipeDirectLighting) {
                return "pt_renderer: pipe != RenderPipeDirectLighting";
        }
        return nullptr;
}
/// Test case: <pt_output_to_file_test>
__dlexport void __callback pt_output_to_file_test_init(struct alg_var_set* envir)
{
}

__dlexport void __callback pt_output_to_file_test_free(struct alg_var_set* envir)
{
}

__dlexport enum DebugPosition* __callback pt_output_to_file_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_ptrenderer_c_pt_radiance_node_compute2
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = false;
        return pos;
}

__dlexport void __callback pt_output_to_file_test(struct alg_var_set* envir)
{
        log_normal("pt output to file test starting");
        struct util_image* img = alg_var_set_use(envir, "target");
        if (img == nullptr) {
                log_normal("parameter: target not passed, test case %s cannot continue", get_function_name());
        }
        int width, height;
        u_image_get_level0_dimension(img, &width, &height);
        FILE* f = fopen("pt_output_to_file_test.ppm", "w");
        fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
        int i, j;
        for (j = 0; j < height; j ++) {
                for (i = 0; i < width; i ++) {
                        unsigned char* px = u_image_read(img, 0, i, j);
                        fprintf(f, "%d %d %d ", px[2], px[1], px[0]);
                }
        }
        fclose(f);

}
/// End Test case: <pt_output_to_file_test>

void pt_radiance_node_compute(struct render_node_ex_impl* self_parent,
                              const struct render_node_ex_impl* input[],
                              const struct render_node_ex_impl* output[])
{
        struct pt_radiance_node* self = (struct pt_radiance_node*) self_parent;
        // Get input node instance <renderable loader> and its context
        const int slot                                  = render_node_radiance_get_input_slot(RenderNodeRenderableLoader);
        struct pt_renderable_loader_node* rdaloader     = (struct pt_renderable_loader_node*) input[slot];
        struct rda_context* context                     = rdaloader->_parent.ops.f_get_result(&rdaloader->_parent);
        // Get geometry data
        uuid_t request_id                               = rda_context_post_request(context, RenderAggregateCullCube,
                                                                                   nullptr, RenderableGeometry);
        rda_context_update(context);
        const int n                                     = rda_context_get_n(context, request_id);
        u_aos_free(&self->aos_geo);
        // pre-check what data does the geometry has
        u_aos_init(&self->aos_geo, UtilAttriVertex | UtilAttriNormal | UtilAttriMatIdList);
        int i;
        for (i = 0; i < n; i ++) {
                struct rda_instance* inst = rda_context_get_i(context, i, request_id);
                struct rda_geometry* geometry = (struct rda_geometry*) rda_instance_source(inst);
                int num_index;
                int* index      = rda_geometry_get_index(geometry, &num_index);
                int num_vertex;
                void* vertex    = rda_geometry_get_vertex(geometry, &num_vertex);
                void* normal    = rda_geometry_get_normal(geometry, &num_vertex);
                int* matid      = rda_geometry_get_material_id_list(geometry, &num_vertex);
                u_aos_accumulate(&self->aos_geo, index, num_index, num_vertex, vertex, normal, matid);
        }

        /* generate streams */
        void* vertex[10];
        int n_vertex = u_aos_get_vertex(&self->aos_geo, vertex, &self->n_streams);
        bool* avail = u_aos_get_availibility(&self->aos_geo);

        int cSizeOfStream[10] = {
                [_AttriVertex] = sizeof(struct point3d),
                [_AttriNormal] = sizeof(struct vector3d),
                [_AttriMatIdList]  = sizeof(int)
        };
        f_Lerp_3 cStreamLerp3[10] = {
                [_AttriVertex] = (f_Lerp_3) lerp_point,
                [_AttriNormal] = (f_Lerp_3) lerp_point,
                [_AttriMatIdList]  = (f_Lerp_3) lerp_matid
        };
        int k, m;
        for (m = 0, k = 0; k < 10; k ++) {
                if (avail[k] == true) {
                        /* @fixme (davis#1#): <ptrenderer> shader should be more elegant */
                        void** shader_var_loc[1] = {nullptr};
                        switch(k) {
                                case _AttriVertex:
                                        shader_var_loc[0] = (void**) &g_position;
                                        break;
                                case _AttriNormal:
                                        shader_var_loc[0] = (void**) &g_normal;
                                        break;
                                case _AttriMatIdList:
                                        shader_var_loc[0] = (void**) &g_mater_id;
                                        break;
                        }

                        u_stream_init(&self->stream[m],
                                      vertex[k], cSizeOfStream[k], n_vertex,
                                      &cSizeOfStream[k], shader_var_loc, 1, nullptr, cStreamLerp3[k]);
                        m ++;
                }
        }
        /* update simplex and accessor utility */
        free_simplex(self->simplex);
        u_access_free(self->acc_stt);

        int num_index, num_simplex;
        int* index = u_aos_get_index(&self->aos_geo, &num_index);
        self->simplex = construct_simplex(self->stream, self->n_streams,
                                          index, num_index, &num_simplex);
        /* @fixme (davis#9#): <pt_radiance_node_compute> hard coded acc_type */
        self->acc_type = UtilAccessorLinear;
        self->acc_stt = u_access_create(self->acc_type, self->simplex, num_simplex);
        u_access_build(self->acc_stt);

        /* create target */
        /* @fixme (davis#2#): <pt_renderer_update> use projective probe to determine the render target of a radiance pass */
        u_image_free(&self->target);
        u_image_init(&self->target, 1, UtilImgRGBA32, 800, 600);
        u_image_alloc(&self->target, 0);

        struct util_image img_rad;
        u_image_init(&img_rad, 1, UtilImgRGBRadiance, 800, 600);
        u_image_alloc(&img_rad, 0);

        render_radiance(self, &img_rad);
/* @fixme (davis#2#): <pt_renderer_update> cheated here... should be in our output */
        render_output(&img_rad, &self->target);
        u_image_free(&img_rad);

        struct alg_var_set params;
        alg_var_set_init(&params);
        alg_var_set_declare(&params, "target", &self->target, sizeof(self->target));
        debugger_invoke_begin();
        debugger_invoke(Debug_ptrenderer_c_pt_radiance_node_compute2, &params);
        debugger_invoke_end();
        alg_var_set_free(&params);
}

void* pt_radiance_node_get_result(const struct render_node_ex_impl* self)
{
        return &((struct pt_radiance_node*) self)->target;
}

void pt_radiance_node_free(struct render_node_ex_impl* self)
{
/* @todo (davis#2#): <pt_radiance_node_free> release resources */
}


// renderable loader node
struct render_node_ex_impl* pt_renderable_loader_node_creator(struct render_node_ex* parent)
{
        struct pt_renderable_loader_node* node = alloc_obj(node);
        zero_obj(node);
        // node->_parent2 = *(struct render_rdaloader*) parent;
        // fill in ops
        struct render_node_ex_ops       ops;
        ops.f_compute           = pt_renderable_loader_node_compute;
        ops.f_free              = pt_renderable_loader_node_free;
        ops.f_get_result        = pt_renderable_loader_node_get_result;
        ops.f_is_compatible     = pt_renderable_loader_node_is_compatible;
        node->_parent.ops       = ops;
        return (struct render_node_ex_impl*) node;
}

char* pt_renderable_loader_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        struct pt_renderable_loader_node* node  = cast(node) self;
        struct render_rdaloader* parent         = cast(parent) render_node_ex_impl_get_ex(self);
        const char* context_name                = render_node_rdaloader_get_context(parent);
        struct rda_context* context = render_tree_retrieve_environment(tree, context_name, RenderEnvRenderable);
        if (!context) {
                return "pt_renderer: !context";
        }
        node->context = context;
        return nullptr;
}

void pt_renderable_loader_node_compute(struct render_node_ex_impl* self,
                                       const struct render_node_ex_impl* input[],
                                       const struct render_node_ex_impl* output[])
{
        // nothing to do
}

void* pt_renderable_loader_node_get_result(const struct render_node_ex_impl* self)
{
        const struct pt_renderable_loader_node* node = cast(node) self;
        return node->context;
}

void pt_renderable_loader_node_free(struct render_node_ex_impl* self)
{
        // nothing to do
}

// render output node
struct render_node_ex_impl* pt_render_output_node_creator(struct render_node_ex* parent)
{
        struct pt_render_output_node* node = alloc_obj(node);
        zero_obj(node);
        // node->_parent2 = *(struct render_output*) parent;
        // fill in ops
        struct render_node_ex_ops       ops;
        ops.f_compute           = pt_render_output_node_compute;
        ops.f_free              = pt_render_output_node_free;
        ops.f_get_result        = pt_render_output_node_get_result;
        ops.f_is_compatible     = pt_render_output_node_is_compatible;
        node->_parent.ops       = ops;
        return (struct render_node_ex_impl*) node;
}

char* pt_render_output_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        struct pt_render_output_node* node      = cast(node) self;
        struct render_output* parent            = cast(parent) render_node_ex_impl_get_ex(self);
        const char* probe_name                  = render_node_output_get_probe(parent);
        struct perspective_probe* probe = render_tree_retrieve_environment(tree, probe_name, RenderEnvProbe);
        if (!probe) {
                return "pt_renderer: !probe";
        }
        if (PerspectiveProbe != projprobe_get_type((struct projection_probe*) probe)) {
                return "pt_renderer: PerspectiveProbe != projprobe_get_type((struct projection_probe*) probe)";
        }
        switch (projprobe_get_output_method((struct projection_probe*) probe)) {
        case GtkRenderRegionOutput:
                break;
        case GtkOpenGLOutput:
                break;
        default:
                return "pt_renderer: invalid output method";
        }
        node->probe = probe;
        return nullptr;
}


#include <gtk/gtk.h>

typedef gboolean (*f_GTK_Display_Callback) (struct _GtkWidget *widget, struct _cairo *cairo, gpointer data);
struct gtk_out {
        f_GTK_Display_Callback  display_callback;
        int                     signal_handler;
        int                     signal_state;
        struct _GtkWidget*      dst_widget;
        enum _cairo_format      format;
        void*                   src_surface;
        int                     width, height;
        int                     x, y;
        int                     stride;
};

#define DRAW_SIGNAL_REMAIN	1
#define DRAW_SIGNAL_NONE	0

static gboolean display_callback(struct _GtkWidget *widget, struct _cairo *cairo, gpointer data);


static struct gtk_out *gtk_out_create(GtkWidget *widget,
                                      int x, int y, int width, int height,
                                      enum ColorMode format, void* src_surface)
{
        struct gtk_out *out = alloc_fix(sizeof *out, 1);
        memset(out, 0, sizeof *out);

        switch(format) {
        case Color8Mode: {
                out->format = CAIRO_FORMAT_A1;
                out->stride = 1;
                break;
        }
        case Color16AMode: {
                out->format = CAIRO_FORMAT_RGB16_565;
                out->stride = 2;
                break;
        }
        case Color24Mode: {
                out->format = CAIRO_FORMAT_RGB24;
                out->stride = 3;
                break;
        }
        case Color32Mode: {
                out->format = CAIRO_FORMAT_ARGB32;
                out->stride = 4;
                break;
        }
        default: {
                log_severe_err_dbg("color format is not supported by the api");
                return nullptr;
        }
        }
        out->display_callback   = display_callback;
        out->dst_widget         = widget;
        out->signal_handler     = g_signal_connect(widget, "draw", G_CALLBACK(display_callback), out);
        out->signal_state       = DRAW_SIGNAL_NONE;
        out->src_surface        = src_surface;
        return out;
}

static void gtk_out_free(struct gtk_out *out)
{
        while(out->signal_state == DRAW_SIGNAL_REMAIN) {
                // log_normal_dbg("Wait for DrawSignal being cleared");
                thread_task_idle(1);
        }
        // log_normal_dbg("signal cleared == 0, clear up");
        g_signal_handler_disconnect(out->dst_widget, out->signal_handler);
        free_fix(out);
}

static void gtk_out_run(struct gtk_out *out)
{
//        gdk_threads_enter ();
        out->signal_state = DRAW_SIGNAL_REMAIN;
        gtk_widget_queue_draw(out->dst_widget);
//        gdk_threads_leave ();
}

static gboolean display_callback(struct _GtkWidget *widget,
                                 struct _cairo *cairo, gpointer data)
{
        struct gtk_out *out = data;
        struct _cairo_surface *co_surface =
                cairo_image_surface_create_for_data(out->src_surface,
                                out->format, out->width, out->height, out->stride);
        cairo_set_source_surface(cairo, co_surface, out->x, out->y);
        cairo_paint(cairo);
        cairo_surface_destroy(co_surface);
        out->signal_state = DRAW_SIGNAL_NONE;
        return 0;
}

void pt_render_output_node_compute(struct render_node_ex_impl* self,
                                   const struct render_node_ex_impl* input[],
                                   const struct render_node_ex_impl* output[])
{
        struct pt_render_output_node* node = cast(node) self;

        const struct render_node_ex_impl* image_node    = input[render_node_output_get_input_slot()];
        struct util_image* input_image                  = image_node->ops.f_get_result(image_node);

        switch (projprobe_get_output_method((struct projection_probe*) node->probe)) {
        case GtkRenderRegionOutput: {
                void* image_buffer              = u_image_read(input_image, 0, 0, 0);
                GtkWidget* target_widget        = projprobe_get_target_screen((struct projection_probe*) node->probe);
                int img_width                   = projprobe_get_width((struct projection_probe*) node->probe);
                int img_height                  = projprobe_get_height((struct projection_probe*) node->probe);
                enum ColorMode colormode        = projprobe_get_colormode((struct projection_probe*) node->probe);
                struct gtk_out* goutput         = gtk_out_create(target_widget, 0, 0, img_width, img_height,
                                                                 colormode, image_buffer);
                gtk_out_run(goutput);
                gtk_out_free(goutput);
                break;
        }
        case GtkOpenGLOutput: {
/* @fixme (davis#9#): <pt_render_out_node_compute> add OpenGL support */
                break;
        }
        }
}

void* pt_render_output_node_get_result(const struct render_node_ex_impl* self)
{
        return ((struct pt_render_output_node*) self)->probe;
}

void pt_render_output_node_free(struct render_node_ex_impl* self)
{
        // nothing to do
}