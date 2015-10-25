#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>

using namespace x3d;
using namespace x3d::usr;


/* KernelEditor */
KernelEditor::KernelEditor() : KernelProxy("KernelEditor")
{
        m_frontend_type = GUIFrontendGTK;
        m_frontend      = new EditorGtkFrontend ();
        m_backend       = nullptr;
}

KernelEditor::~KernelEditor ()
{
        delete m_frontend;
        delete m_backend;
}

void KernelEditor::close()
{
        get_subjected_kernel()->stop();
}

int KernelEditor::on_init ( int argc, char** argv, KernelEnvironment *env )
{
        env->declare ( "argc", env->make<int>(argc) );
        env->declare ( "argv", env->make<char**>(argv) );

        m_backend->open ();

        if (!m_frontend->init(argc, argv, m_backend, env)) {
                goto fail;
        }

        if (!m_backend->load_state(m_state_file)) {
                log_mild_err_dbg("couldn't load editor state from: %s", m_state_file.c_str());
        }
        return 0;
fail:
        kernel_panic();
        return 1;
}

int KernelEditor::on_rest_init ( KernelEnvironment *env )
{
        if (!m_frontend->load(m_backend, env)) {
                goto fail;
        }
        return 0;
fail:
        kernel_panic();
        return 1;
}

struct data_info {
        EditorFrontend*         frontend;
        EditorBackend*          backend;
        KernelEnvironment*      env;
};

static void* gui_thread(struct data_info* data)
{
        data->frontend->loop(data->backend, data->env);
        log_normal_dbg("gui loop exit");
        free_fix(data);
        return nullptr;
}

int KernelEditor::on_loop_init ( KernelEnvironment *env )
{
        struct data_info* info = (struct data_info*) alloc_fix ( sizeof *info, 1 );

        if (!m_frontend->end_init(m_backend, env)) {
                goto fail;
        }

        info->frontend  = m_frontend;
        info->backend   = m_backend;
        info->env       = env;

        m_loop_task = thread_run_task("gui_thread", (f_Thread_Handler) gui_thread, info, nullptr);
        return 0;
fail:
        free_fix ( info );
        kernel_panic ();
        return 1;
}

int KernelEditor::on_loop(KernelEnvironment* env)
{
        if (!m_backend->is_open()) {
                this->get_subjected_kernel()->stop();
                return 1;
        }

        this->m_backend->update();
        return 0;
}

int KernelEditor::on_loop_free(KernelEnvironment* env)
{
        return 0;
}

int KernelEditor::on_free(KernelEnvironment* env)
{
        if (!m_backend->save_state(m_state_file)) {
                log_mild_err_dbg("couldn't save editor state to: %s", m_state_file.c_str());
        }
        if (!m_frontend->free(m_backend, env)) {
                goto fail;
        }
        thread_sync_with_task(m_loop_task);
        log_normal_dbg("the gui thread has exited safely");
        return 0;
fail:
        kernel_panic();
        return 1;
}

void KernelEditor::register_editor_backend(EditorBackend* backend)
{
        if (m_backend) {
                delete m_backend;
        }
        m_backend = backend;
}

void KernelEditor::register_editor_frontend (GUIFrontendType type, EditorFrontend *frontend)
{
        if ( this->m_frontend && !this->m_frontend->is_custum () ) {
                delete this->m_frontend;
        }

        switch ( type ) {
        case GUIFrontendNone:
        case GUIFrontendGTK: {
                this->m_frontend = new EditorGtkFrontend ();
                break;
        }
        case GUIFrontendQT: {
                this->m_frontend = new EditorQtFrontend ();
                break;
        }
        case GUIFrontendWin32: {
                this->m_frontend = new EditorWin32Frontend ();
                break;
        }
        case GUIFrontendPureSDL: {
                this->m_frontend = new EditorPureSdlFrontend ();
                break;
        }
        case GUIFrontendCUSTOM: {
                this->m_frontend = frontend;
                break;
        }
        default: {
                this->m_frontend = new EditorGtkFrontend ();
                break;
        }
        }
}


/* EditorBackend */
EditorBackend::EditorBackend ( void )
{
        for ( int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++ ) {
                this->m_activex[i].clear ();
        }
        this->m_id = alg_gen_uuid ();
        this->m_is_open = false;
}

EditorBackend::~EditorBackend ( void )
{
        for ( int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorBackendActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        delete (*activex);
                }
                this->m_activex[i].clear ();
        }
}

void EditorBackend::update ( void )
{
        for (int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++) {
                for (list<EditorBackendActiveX*>::iterator activex = this->m_activex[i].begin ();
                     activex != this->m_activex[i].end (); ++ activex) {
                        (*activex)->preupdate ();
                }
        }
        for (int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++) {
                for (list<EditorBackendActiveX*>::iterator activex = this->m_activex[i].begin ();
                     activex != this->m_activex[i].end (); ++ activex) {
                        (*activex)->update ();
                }
        }
}

bool EditorBackend::add_activex(EditorBackendActiveX* activex)
{
        EditorBackendActiveX::EditActiveXType type = activex->get_type();
        bool found = (std::find(m_activex[type].begin(), m_activex[type].end(), activex) != m_activex[type].end());
        if (!found) {
                activex->notify_add(this);
                m_activex[type].push_back(activex);
                return true;
        } else {
                log_mild_err("ActiveX %s already exists in the backend editor", activex->get_name().c_str());
                return false;
        }
}

EditorBackendActiveX *EditorBackend::find_activex ( EditorBackendActiveX::EditActiveXType type, string name )
{
        list<EditorBackendActiveX*>::iterator activex =
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

bool EditorBackend::remove_activex ( EditorBackendActiveX::EditActiveXType type, string name )
{
        list<EditorBackendActiveX*>::iterator activex =
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

void EditorBackend::dispatch_signal ( void )
{
        for ( int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorBackendActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        (*activex)->dispatch ();
                }
        }
}

bool EditorBackend::load_state ( string filename )
{
        struct serializer s;
        serial_init(&s);/*
        if (!serial_import(&s)) {
                log_severe_err_dbg("couldn't load in editor state from: %s", filename.c_str());
                return false;
        }*/

        for ( int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorBackendActiveX*>::iterator activex = this->m_activex[i].begin ();
                      activex != this->m_activex[i].end ();
                      ++ activex ) {
                        (*activex)->load ( &s );
                }
        }
        return true;
}

bool EditorBackend::save_state ( string filename )
{
        struct serializer s;
        serial_init ( &s );

        for ( int i = 0; i < EditorBackendActiveX::c_NumActiveXType; i ++ ) {
                for ( list<EditorBackendActiveX*>::iterator activex = this->m_activex[i].begin ();
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

/* EditorBackendActiveX */
EditorBackendActiveX::EditorBackendActiveX(string name, int size, EditActiveXType type) :
                m_size (size), m_type (type)
{
        m_name          = name;
        m_bufcount      = 0;
        x3d::thread_init_trap ( &m_block_driver );
        m_dirty         = false;
};

EditorBackendActiveX::~EditorBackendActiveX()
{
}

void EditorBackendActiveX::notify_add(EditorBackend *e)
{
        m_state   = e->get_global_state ();
        m_edit_id = e->get_id ();
        on_adding ();
}

string EditorBackendActiveX::get_name() const
{
        return m_name;
}

EditorBackendActiveX::EditActiveXType EditorBackendActiveX::get_type() const
{
        return m_type;
}

KernelEnvironment* EditorBackendActiveX::get_state_buffer() const
{
        if (m_state == nullptr)
                log_severe_err_dbg("state buffer is empty!");
        return m_state;
}

bool EditorBackendActiveX::is_dirty() const
{
        return m_dirty;
}

bool EditorBackendActiveX::operator==(const EditorBackendActiveX& rhs)
{
        return m_type == rhs.get_type() && m_name == rhs.get_name();
}

bool EditorBackendActiveX::operator!=(const EditorBackendActiveX& rhs)
{
        return !(*this == rhs);
}


/* front-back buffer utilities */
int EditorBackendActiveX::on_front_buf ( void ) const
{
        return m_bufcount & 1;
}

int EditorBackendActiveX::on_back_buf  ( void ) const
{
        return (m_bufcount + 1) & 1;
}

void EditorBackendActiveX::swap_buf ( void )
{
        m_bufcount ++;
}

void EditorBackendActiveX::wait_for_update ( void )
{
        x3d::thread_trap_on_task ( &m_block_driver );
}

void EditorBackendActiveX::unwait ( void )
{
        x3d::thread_untrap_task ( &m_block_driver );
}

/* dirt-mark utilities */
void EditorBackendActiveX::mark_dirty ( void )
{
        m_dirty = true;
}

void EditorBackendActiveX::unmark_dirty ( void )
{
        m_dirty = false;
}
