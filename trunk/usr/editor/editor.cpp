#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorbackend.hpp>

using namespace x3d;
using namespace x3d::usr;


/* KernelEditor */
KernelEditor::KernelEditor ( Editor *e ) :
        KernelProxy ( "KernelEditor" ),
        m_editor (e)
{
        this->m_frontend_type = GUI_FONTEND_GTK;
        this->m_frontend = new EditorGtkFrontend ();
}

KernelEditor::~KernelEditor ()
{
        delete this->m_frontend;
}

int KernelEditor::on_init ( int argc, char **argv, KernelEnvironment *env )
{
        env->declare<int> ( "argc", &argc );
        env->declare<char **> ( "argv", &argv );

        if ( !this->m_frontend->init ( argc, argv, this->m_editor, env ) ) {
                goto fail;
        }
        return 0;
fail:
        kernel_panic ();
        return 1;
}

int KernelEditor::on_rest_init ( KernelEnvironment *env )
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

static void *gui_thread ( struct data_info *data )
{
        data->backend->loop ( data->e, data->env );
        free_fix ( data );
        return nullptr;
}

int KernelEditor::on_loop_init ( KernelEnvironment *env )
{
        struct data_info *info = cast(info) alloc_fix ( sizeof *info, 1 );

        if ( !this->m_frontend->end_init ( this->m_editor, env ) ) {
                goto fail;
        }

        info->backend = this->m_frontend;
        info->e = this->m_editor;
        info->env = env;

        this->m_loop_task =
                thr_run_task ( (f_Thread_Handler) gui_thread, info, nullptr );
        return 0;
fail:
        free_fix ( info );
        kernel_panic ();
        return 1;
}

int KernelEditor::on_loop ( KernelEnvironment *env )
{
        this->m_editor->update ();
        return 0;
}

int KernelEditor::on_loop_free ( KernelEnvironment *env )
{
        return 0;
}

int KernelEditor::on_free ( KernelEnvironment *env )
{
        if ( !this->m_frontend->free ( this->m_editor, env ) ) {
                goto fail;
        }
        return 0;
fail:
        kernel_panic ();
        return 1;
}

void KernelEditor::register_gui_frontend ( GUI_FONTEND_IDR type, EditorFrontend *backend )
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
        for ( int i = 0; i < c_NumActiveXType; i ++ ) {
                this->m_activex[i].clear ();
        }
        this->m_id = alg_gen_uuid ();
}

Editor::~Editor ( void )
{
        for ( int i = 0; i < c_NumActiveXType; i ++ ) {
                list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                while ( activex != this->m_activex[i].end () ) {
                        delete (*activex);
                        ++ activex;
                }
                this->m_activex[i].clear ();
        }
}

void Editor::update ( void )
{
        for ( int i = 0; i < c_NumActiveXType; i ++ ) {
                list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                while ( activex != this->m_activex[i].end () ) {
                        (*activex)->update ();
                        ++ activex;
                }
        }
}

void Editor::add_activex ( EditorActiveX *activex )
{
        EDIT_ACTIVEX_IDR type = activex->get_type ();
        activex->notify_add ( this );
        this->m_activex[type].push_back ( activex );
}

EditorActiveX *Editor::find_activex ( EDIT_ACTIVEX_IDR type, char *name )
{
        list<EditorActiveX*>::iterator activex =
                this->m_activex[type].begin ();
        while ( activex != this->m_activex[type].end () ) {
                if ( !strcmp ( name, (*activex)->get_name () ) ) {
                        return *activex;
                }
                ++ activex;
        }
        log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s", type, name );
        return nullptr;
}

bool Editor::remove_activex ( EDIT_ACTIVEX_IDR type, char *name )
{
        list<EditorActiveX*>::iterator activex =
                this->m_activex[type].begin ();
        while ( activex != this->m_activex[type].end () ) {
                if ( !strcmp ( name, (*activex)->get_name () ) ) {
                        this->m_activex[type].erase ( activex );
                        return true;
                }
                ++ activex;
        }
        log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s", type, name );
        return false;
}

void Editor::dispatch_signal ( void )
{
        for ( int i = 0; i < c_NumActiveXType; i ++ ) {
                list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                while ( activex != this->m_activex[i].end () ) {
                        (*activex)->dispatch ();
                        ++ activex;
                }
        }
}

bool Editor::load_state ( char *filename )
{
        struct serializer s;
        serial_init ( &s );
        if ( !serial_import ( filename, &s ) ) {
                log_severe_err_dbg ( "couldn't load in editor state from: %s", filename );
                return false;
        }

        for ( int i = 0; i < c_NumActiveXType; i ++ ) {
                list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                while ( activex != this->m_activex[i].end () ) {
                        (*activex)->load ( &s );
                        ++ activex;
                }
        }
        return true;
}

bool Editor::save_state ( char *filename )
{
        struct serializer s;
        serial_init ( &s );

        for ( int i = 0; i < c_NumActiveXType; i ++ ) {
                list<EditorActiveX*>::iterator activex = this->m_activex[i].begin ();
                while ( activex != this->m_activex[i].end () ) {
                        (*activex)->save ( &s );
                        ++ activex;
                }
        }

        if ( !serial_export ( filename, &s ) ) {
                log_severe_err_dbg ( "couldn't export editor state to: %s", filename );
                return false;
        }
        return true;
}

