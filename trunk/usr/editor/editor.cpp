#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>

using namespace x3d;
using namespace x3d::usr;


/* KernelEditor */
EditorBackend::EditorBackend ( Editor *e ) :
        KernelProxy ( "KernelEditor" ),
        m_editor (e)
{
        this->m_frontend_type = GUI_FONTEND_GTK;
        this->m_frontend = new EditorGtkFrontend ();
}

EditorBackend::~EditorBackend ()
{
        delete m_frontend;
}

int EditorBackend::on_init ( int argc, char** argv, KernelEnvironment *env )
{
        env->declare ( "argc", env->make<int>(argc) );
        env->declare ( "argv", env->make<char**>(argv) );

        this->m_editor->open ();

        if ( !this->m_frontend->init ( argc, argv, this->m_editor, env ) ) {
                goto fail;
        }

        if ( !this->m_editor->load_state ( this->m_state_file ) ) {
                log_mild_err_dbg ( "couldn't load editor state from: %s",
                                   this->m_state_file.c_str () );
        }
        return 0;
fail:
        kernel_panic ();
        return 1;
}

int EditorBackend::on_rest_init ( KernelEnvironment *env )
{
        if ( !this->m_frontend->load ( this->m_editor, env ) ) {
                goto fail;
        }
        return 0;
fail:
        kernel_panic ();
        return 1;
}

struct data_info {
        EditorFrontend*          backend;
        KernelEnvironment*      env;
        Editor*                 e;
};

static void* gui_thread(struct data_info* data)
{
        data->backend->loop(data->e, data->env);
        log_normal_dbg("gui loop exit");
        free_fix(data);
        return nullptr;
}

int EditorBackend::on_loop_init ( KernelEnvironment *env )
{
        struct data_info* info = (struct data_info*) alloc_fix ( sizeof *info, 1 );

        if ( !this->m_frontend->end_init ( this->m_editor, env ) ) {
                goto fail;
        }

        info->backend = this->m_frontend;
        info->e = this->m_editor;
        info->env = env;

        this->m_loop_task = thread_run_task("gui_thread", (f_Thread_Handler) gui_thread, info, nullptr);
        return 0;
fail:
        free_fix ( info );
        kernel_panic ();
        return 1;
}

int EditorBackend::on_loop ( KernelEnvironment *env )
{
        if ( !this->m_editor->is_open () ) {
                this->get_subjected_kernel()->stop();
                return 1;
        }

        this->m_editor->update ();
        return 0;
}

int EditorBackend::on_loop_free ( KernelEnvironment *env )
{
        return 0;
}

int EditorBackend::on_free ( KernelEnvironment *env )
{
        if ( !this->m_editor->save_state ( this->m_state_file ) ) {
                log_mild_err_dbg ( "couldn't save editor state to: %s",
                                   this->m_state_file.c_str () );
        }
        if ( !this->m_frontend->free ( this->m_editor, env ) ) {
                goto fail;
        }
        thread_sync_with_task(m_loop_task);
        log_normal_dbg("the gui thread has exited safely");
        return 0;
fail:
        kernel_panic ();
        return 1;
}

void EditorBackend::register_gui_frontend ( GUI_FONTEND_IDR type, EditorFrontend *backend )
{
        if ( this->m_frontend && !this->m_frontend->is_custum () ) {
                delete this->m_frontend;
        }

        switch ( type ) {
        case GUI_FONTEND_NONE:
        case GUI_FONTEND_GTK: {
                this->m_frontend = new EditorGtkFrontend ();
                break;
        }
        case GUI_FONTEND_QT: {
                this->m_frontend = new EditorQtFrontend ();
                break;
        }
        case GUI_FONTEND_WIN32: {
                this->m_frontend = new EditorWin32Frontend ();
                break;
        }
        case GUI_FONTEND_PURE_SDL: {
                this->m_frontend = new EditorPureSdlFrontend ();
                break;
        }
        case GUI_FONTEND_CUSTUM: {
                this->m_frontend = backend;
                break;
        }
        default: {
                this->m_frontend = new EditorGtkFrontend ();
                break;
        }
        }
}


/* Editor */
Editor::Editor ( void )
{
        for ( int i = 0; i < EditorActiveX::c_NumActiveXType; i ++ ) {
                this->m_activex[i].clear ();
        }
        this->m_id = alg_gen_uuid ();
        this->m_is_open = false;
}

Editor::~Editor ( void )
{
        for ( int i = 0; i < EditorActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        delete (*activex);
                }
                this->m_activex[i].clear ();
        }
}

void Editor::update ( void )
{
        for ( int i = 0; i < EditorActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        (*activex)->update ();
                }
        }
}

void Editor::add_activex ( EditorActiveX *activex )
{
        EditorActiveX::EDIT_ACTIVEX_IDR type = activex->get_type ();
        activex->notify_add ( this );
        this->m_activex[type].push_back ( activex );
}

EditorActiveX *Editor::find_activex ( EditorActiveX::EDIT_ACTIVEX_IDR type, string name )
{
        list<EditorActiveX*>::iterator activex =
                this->m_activex[type].begin ();
        while ( activex != this->m_activex[type].end () ) {
                if ( name == (*activex)->get_name () ) {
                        return *activex;
                }
                ++ activex;
        }
        log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s",
                           type, name.c_str () );
        return nullptr;
}

bool Editor::remove_activex ( EditorActiveX::EDIT_ACTIVEX_IDR type, string name )
{
        list<EditorActiveX*>::iterator activex =
                this->m_activex[type].begin ();
        while ( activex != this->m_activex[type].end () ) {
                if ( name == (*activex)->get_name () ) {
                        this->m_activex[type].erase ( activex );
                        return true;
                }
                ++ activex;
        }
        log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s",
                           type, name.c_str () );
        return false;
}

void Editor::dispatch_signal ( void )
{
        for ( int i = 0; i < EditorActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        (*activex)->dispatch ();
                }
        }
}

bool Editor::load_state ( string filename )
{
        struct serializer s;
        serial_init(&s);/*
        if (!serial_import(&s)) {
                log_severe_err_dbg("couldn't load in editor state from: %s", filename.c_str());
                return false;
        }*/

        for ( int i = 0; i < EditorActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        (*activex)->load ( &s );
                }
        }
        return true;
}

bool Editor::save_state ( string filename )
{
        struct serializer s;
        serial_init ( &s );

        for ( int i = 0; i < EditorActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        (*activex)->save ( &s );
                }
        }

        /*int size;
        uint8_t* stream = serial_export(&s, &size);
        if (false) {
                log_severe_err_dbg ( "couldn't export editor state to: %s", filename.c_str());
                return false;
        }*/
        return true;
}

/* EditorActiveX */
EditorActiveX::EditorActiveX ( string name, int size, EDIT_ACTIVEX_IDR type ) :
                m_size (size), m_type (type)
{
        m_name          = name;
        m_bufcount      = 0;
        x3d::thread_init_trap ( &m_block_driver );
        m_dirty         = false;
};

EditorActiveX::~EditorActiveX ()
{
}

void EditorActiveX::notify_add ( Editor *e )
{
        m_state   = e->get_global_state ();
        m_edit_id = e->get_id ();
        on_adding ();
}

string EditorActiveX::get_name ( void ) const
{
        return m_name;
}

EditorActiveX::EDIT_ACTIVEX_IDR EditorActiveX::get_type ( void ) const
{
        return m_type;
}

KernelEnvironment* EditorActiveX::get_state_buffer ( void ) const
{
        if ( m_state == nullptr )
                log_severe_err_dbg ( "state buffer is empty!" );
        return m_state;
}

bool EditorActiveX::is_dirty ( void ) const
{
        return m_dirty;
}

/* front-back buffer utilities */
int EditorActiveX::on_front_buf ( void ) const
{
        return m_bufcount & 1;
}

int EditorActiveX::on_back_buf  ( void ) const
{
        return (m_bufcount + 1) & 1;
}

void EditorActiveX::swap_buf ( void )
{
        m_bufcount ++;
}

void EditorActiveX::wait_for_update ( void )
{
        x3d::thread_trap_on_task ( &m_block_driver );
}

void EditorActiveX::unwait ( void )
{
        x3d::thread_untrap_task ( &m_block_driver );
}

/* dirt-mark utilities */
void EditorActiveX::mark_dirty ( void )
{
        m_dirty = true;
}

void EditorActiveX::unmark_dirty ( void )
{
        m_dirty = false;
}
