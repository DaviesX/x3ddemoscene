#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_renderaggregate.hpp>
#include <usr/usr_renderer.hpp>

namespace x3d
{

namespace usr
{

class RenderConfigActiveX::RenderConfigInt
{
public:
        RenderConfigInt();
        ~RenderConfigInt();
public:
        struct Tab {
                struct Value {
                        string          name;
                        int32_t         data[8];
                };
                set<struct Value>       tab_values;
                string                  tab_name;
        };
        set<struct Tab>         m_tabs;

        struct Command {
                void push(string *tab_name)
                {
                        struct Tab::Value command;
                        command.name = *tab_name;
                        stack.push_back(command);
                }
                void push(string *value_type, void *data)
                {
                        struct Tab::Value command;
                        command.name = *value_type;
                        memcpy(command.data, data, sizeof(int32_t)*8);
                        stack.push_back(command);
                }
                void clear()
                {
                        stack.clear();
                }
                vector<struct Tab::Value>       stack;
        } m_command;

        struct Error {
                vector<string>  error_list;
                f_Notify_Error  f_error;
                void*           d_error;
        } m_error[2];

        RenderTree              m_static;
        RenderTree              m_interactive;
        string                  c_ProbeName = "default_probe";
        string                  c_RenderContextName = "default_context";
};

RenderConfigActiveX::RenderConfigInt::RenderConfigInt()
{
        // Put a default tree onto it
        RenderOutput soutput("default_output", c_ProbeName);
        RenderRadiance sradiance("default_radiance", RenderPipeDirectLighting);
        RenderableLoader srdaloader("default_renderable_context", c_RenderContextName, RenderAggregateLinear);
        RenderNode* root = m_static.create_root();
        m_static.insert_node(root, soutput.get_node());
        m_static.insert_node(soutput.get_node(), sradiance.get_node());
        m_static.insert_node(sradiance.get_node(), srdaloader.get_node());
        
        RenderOutput ioutput("default_output", c_ProbeName);
        RenderRadiance iradiance("default_radiance", RenderPipeImageSpace);
        RenderableLoader irdaloader("default_renderable_context", c_RenderContextName, RenderAggregateStaticBVH);
        root = m_interactive.create_root();
        m_interactive.insert_node(root, ioutput.get_node());
        m_interactive.insert_node(ioutput.get_node(), iradiance.get_node());
        m_interactive.insert_node(iradiance.get_node(), irdaloader.get_node());
}

RenderConfigActiveX::RenderConfigInt::~RenderConfigInt()
{
}

RenderConfigActiveX::RenderConfigActiveX(string name) :
        EditorBackendActiveX(name, sizeof(RenderConfigActiveX), EditActiveXRenderConfig)
{
        pimpl = new RenderConfigInt();

        int i;
        for (i = 0; i < 2; i ++) {
                pimpl->m_error[i].error_list.clear();
                pimpl->m_error[i].f_error  = nullptr;
                pimpl->m_error[i].f_error  = nullptr;
        }
}

RenderConfigActiveX::~RenderConfigActiveX()
{
        delete this->pimpl;
}

void RenderConfigActiveX::on_adding()
{
        // make a default renderer
        KernelEnvironment* state = this->get_state_buffer();
        /*struct x3d::renderer* renderer =
                (struct x3d::renderer*) state->use ( c_RenderConfig );
        if ( renderer ) {
                x3d::free_renderer ( renderer );
        }
        renderer = x3d::create_renderer ( RENDERER_PATH_TRACER, nullptr );
        state->declare ( c_Renderer, renderer );*/
        state->declare(c_RenderConfig, this);
}

void RenderConfigActiveX::dispatch()
{
        {
                RenderConfigInt::Error *t_error = &pimpl->m_error[on_back_buf()];
                if (t_error->f_error) {
                        while (!t_error->error_list.empty()) {
                                string msg = t_error->error_list.back();
                                t_error->f_error(msg, this, t_error->d_error);
                                t_error->error_list.pop_back();
                        }
                }
        }
}

void RenderConfigActiveX::update()
{;
        KernelEnvironment* state = get_state_buffer();
        RenderRegionActiveX* region = (RenderRegionActiveX*) state->use(c_RenderRegion);
        ProjectionProbe* probe = region->get_probe();
        WorldDataActiveX* world_data = (WorldDataActiveX*) state->use(c_WorldData);
        World& world = world_data->get_world();
        /* atomically swap the state buffer */
        wait_for_update ();
        swap_buf();
        unwait();
        /* update render tree */
        pimpl->m_static.clear_environment_variables();
        pimpl->m_static.set_environment_variable(RenderEnvProbe, pimpl->c_ProbeName, probe->get_core_resource());
        pimpl->m_static.set_environment_variable(RenderEnvRenderable, pimpl->c_RenderContextName,
                                                 world.get_render_aggregate()->get_core_resource());
        
        pimpl->m_interactive.clear_environment_variables();
        pimpl->m_interactive.set_environment_variable(RenderEnvProbe, pimpl->c_ProbeName, probe->get_core_resource());
        pimpl->m_interactive.set_environment_variable(RenderEnvRenderable, pimpl->c_RenderContextName,
                                                      world.get_render_aggregate()->get_core_resource());
}

void RenderConfigActiveX::load ( struct serializer *s )
{
}

void RenderConfigActiveX::save ( struct serializer *s )
{
}

void RenderConfigActiveX::set_config_tab ( string tab_name )
{
}

void RenderConfigActiveX::set_config_value ( string value, void *data )
{
}

bool RenderConfigActiveX::apply ( void )
{
        return true;
}

void RenderConfigActiveX::cancel ( void )
{
}

void* RenderConfigActiveX::checkout_value ( string tab_name, string value )
{
        return nullptr;
}

void RenderConfigActiveX::bind_callback ( string signal, f_Generic callback, void *data )
{
        wait_for_update();
        if ( "notify_error" == signal ) {
                pimpl->m_error[on_front_buf()].f_error = (f_Notify_Error) callback;
                pimpl->m_error[on_front_buf()].d_error = data;
        } else {
                log_mild_err_dbg ( "no such signal as: %s", signal.c_str () );
                return ;
        }
        unwait();
}

RenderTree* RenderConfigActiveX::get_render_tree(RenderTreeConfigType type) const
{
        switch (type) {
        case RenderTreeStatic:
                return &pimpl->m_static;
        case RenderTreeInteractive:
                return &pimpl->m_interactive;
        }
}

} // namespace usr

} // namespace x3d
