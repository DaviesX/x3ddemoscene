#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_renderer.hpp>

namespace x3d
{

namespace usr
{

class RenderConfigActiveX::RenderConfigInt
{
public:
        RenderConfigInt ();
        ~RenderConfigInt ();
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
                void push ( string *tab_name )
                {
                        struct Tab::Value command;
                        command.name = *tab_name;
                        stack.push_back ( command );
                }
                void push ( string *value_type, void *data )
                {
                        struct Tab::Value command;
                        command.name = *value_type;
                        memcpy ( command.data, data, sizeof(int32_t)*8 );
                        stack.push_back ( command );
                }
                void clear ( void )
                {
                        stack.clear ();
                }
                vector<struct Tab::Value>       stack;
        } m_command;

        struct Error {
                vector<string>  error_list;
                f_Notify_Error  f_error;
                void*           d_error;
        } m_error[2];

        RenderTree              m_tree;
        Renderer*               m_renderer;
};

RenderConfigActiveX::RenderConfigInt::RenderConfigInt ()
{
        m_renderer = new Renderer(Renderer::RENDERER_PATH_TRACER);
}

RenderConfigActiveX::RenderConfigInt::~RenderConfigInt ()
{
}

RenderConfigActiveX::RenderConfigActiveX ( string name ) :
        EditorActiveX ( name, sizeof(RenderConfigActiveX), EDIT_ACTIVEX_RENDER_CONFIG )
{
        pimpl = new RenderConfigInt();

        int i;
        for ( i = 0; i < 2; i ++ ) {
                pimpl->m_error[i].error_list.clear ();
                pimpl->m_error[i].f_error  = nullptr;
                pimpl->m_error[i].f_error  = nullptr;
        }
}

RenderConfigActiveX::~RenderConfigActiveX ( void )
{
        delete this->pimpl;
}

void RenderConfigActiveX::on_adding ( void )
{
        // make a default renderer
        KernelEnvironment* state = this->get_state_buffer ();
        /*struct x3d::renderer* renderer =
                (struct x3d::renderer*) state->use ( c_RenderConfig );
        if ( renderer ) {
                x3d::free_renderer ( renderer );
        }
        renderer = x3d::create_renderer ( RENDERER_PATH_TRACER, nullptr );
        state->declare ( c_Renderer, renderer );*/
        state->declare ( c_RenderConfig, this );
}

void RenderConfigActiveX::dispatch ( void )
{
        wait_for_update ();
        {
                RenderConfigInt::Error *t_error = &pimpl->m_error[on_back_buf()];
                if ( t_error->f_error ) {
                        while ( !t_error->error_list.empty () ) {
                                string msg = t_error->error_list.back ();
                                t_error->f_error ( msg, this, t_error->d_error );
                                t_error->error_list.pop_back ();
                        }
                }
        }
        unwait ();
}

void RenderConfigActiveX::update ( void )
{;
        /* atomically swap the state buffer */
        wait_for_update ();
        swap_buf ();
        unwait ();

        pimpl->m_renderer->update(&pimpl->m_tree);
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
        if ( "notify_error" == signal ) {
                pimpl->m_error[on_front_buf()].f_error = (f_Notify_Error) callback;
                pimpl->m_error[on_front_buf()].d_error = data;
        } else {
                log_mild_err_dbg ( "no such signal as: %s", signal.c_str () );
                return ;
        }
}

Renderer* RenderConfigActiveX::get_renderer()
{
        return pimpl->m_renderer;
}

} // namespace usr

} // namespace x3d
