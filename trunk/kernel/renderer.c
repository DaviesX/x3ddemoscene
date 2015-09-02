#include <system/log.h>
#include <system/symlib.h>
#include <x3d/renderer.h>
#include <renderer/rendererinsmod.h>


#define MAX_RENDER_CONTEXT		32

struct renderer_ops {
        f_Rendererinsmod        f_insmod;
        f_Rendererrmmod         f_rmmod;
};

#define c_Num_Render_Node_Type          32
#define c_Num_Render_Node_Impl          32

struct renderer_node_ops_metadata {
        f_Render_Node_Creator   f_creator[c_Num_Render_Node_Type][c_Num_Render_Node_Impl];
        int                     count[c_Num_Render_Node_Type];
};

static struct renderer_ops                      g_rend_ops = {0};
static struct renderer_node_ops_metadata        g_rend_ops_metadata = {0};


void renderer_kernel_init()
{
        g_rend_ops.f_insmod();
}

void renderer_kernel_free()
{
        g_rend_ops.f_rmmod();
}

static bool match_mod_abi(void* data, const char* symbol)
{
        const char* base_str = data;
        return !strncmp(symbol, base_str, strlen(base_str));
}

bool renderer_import(struct symbol_set* symbols)
{
        struct renderer_ops *t_ops = &g_rend_ops;
        bool ret = true;

        if (!(t_ops->f_insmod = (f_Rendererinsmod) symlib_ret_abi2(symbols, "rendererinsmod_x3d", match_mod_abi))) {
                log_severe_err_dbg("fail to retrieve abi: rendererinsmod_x3d");
                ret = false;
        }
        if (!(t_ops->f_rmmod = (f_Rendererrmmod) symlib_ret_abi2(symbols, "rendererrmmod_x3d", match_mod_abi))) {
                log_severe_err_dbg("fail to retrieve abi: rendererinsmod_x3d");
                ret = false;
        }
        return ret;
}

/* renderer's */
void renderer_render(struct render_tree* tree)
{
}

/* render_node_factory's */
void renderer_install_render_node_factory(int id, enum RenderNodeType type, f_Render_Node_Creator creator)
{
        g_rend_ops_metadata.f_creator[type][id] = creator;
}

void renderer_remove_render_node_factory(int id, enum RenderNodeType type)
{
        g_rend_ops_metadata.f_creator[type][id] = nullptr;
}

void renderer_finalize_render_node_factory()
{
        int i;
        for (i = 0; i < c_Num_Render_Node_Type; i ++) {
                int j;
                for (j = 0; g_rend_ops_metadata.f_creator[i][j] != nullptr; j ++);
                g_rend_ops_metadata.count[i] = j;
        }
}

void render_node_ex_init(struct render_node_ex* self, enum RenderNodeType type)
{
        self->type = type;
        int i;
        for (i = 0; i < g_rend_ops_metadata.count[type]; i ++) {
                self->metainst[i] = g_rend_ops_metadata.f_creator[type][i](self);
                self->metainst[i]->ref = self;
        }
}

void render_node_ex_free(struct render_node_ex* self)
{
        int j;
        for (j = 0; j < c_Num_Render_Node_Type; j ++) {
                int i;
                for (i = 0; i < g_rend_ops_metadata.count[j]; i ++) {
                        self->metainst[i]->ops.f_free(self->metainst[i]);
                }
        }
}

struct render_node_ex* render_node_ex_impl_get_ex(struct render_node_ex_impl* self)
{
        return self->ref;
}

struct render_node_ex_impl* render_node_ex_get_ex_impl(struct render_node_ex* self, int id)
{
        return self->metainst[id];
}
