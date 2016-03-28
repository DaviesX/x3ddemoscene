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
#include "pathtracerimpl.h"
#include "trianglebuffer.h"
#include "vbo.h"
#include "fbo.h"
#include <x3d/colorspectrum.h>

struct rda_context;

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

        // prepare geometric data into geometry cache
        struct geomcache gc;
        geomcache_init(&gc, UtilAttriVertex | UtilAttriNormal | UtilAttriMatIdList);
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
                geomcache_accumulate(&gc, index, num_index, num_vertex, vertex, normal, matid);
        }

        // prepare material data
        int n_materials;
        struct bsdf_model** materials = rda_context_get_materials(context, &n_materials);

        // prepare illuminance data
        int n_lights;
        struct light** lights = rda_context_get_lights(context, &n_lights);

        /* @fixme (davis#9#): <pt_radiance_node_compute> hard coded acc_type */
        // create spatial accessor
        int n_objects;
        struct box3d* simplexes = geomcache_export_simplexes(&gc, &n_objects);
        struct spatial_linear li_acc;
        u_linear_init(&li_acc, simplexes, n_objects);
        u_access_build(&li_acc._parent);

        /* create targets */
        /* @fixme (davis#2#): <pt_renderer_update> use projective probe to determine the render target of a radiance pass */
        struct util_image radiance_target;
        u_image_init(&radiance_target, 1, UtilImgRGBRadiance, 800, 600);
        u_image_alloc(&radiance_target, 0);

        // render radiance
        struct pathtracer pt;
        pathtracer_init(&pt);

        pathtracer_set_bsdfs(&pt, materials, n_materials);
        pathtracer_set_lights(&pt, lights, n_lights);
        pathtracer_set_geometries(&pt, &gc, &li_acc._parent);

        pathtracer_render(&pt, &radiance_target);
        pathtracer_free(&pt);

        u_image_free(&self->target);
        self->target = radiance_target;
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
