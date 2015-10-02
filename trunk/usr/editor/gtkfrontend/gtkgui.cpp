#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

// properties exchanges
class EditorGtkFrontend::EditorBackendGtkFrontendInt
{
public:
        EditorBackendGtkFrontendInt(EditorGtkFrontend* frontend);
        ~EditorBackendGtkFrontendInt();

        EditorBackendMode              m_editor_mode;
        EditorBackend*                 m_editor;
        KernelEnvironment*      m_env;
        GtkBuilder*             m_builder;

        SplashScreen*           m_splash_screen;
        MainEditor*             m_main_editor;
        EntityEditor*           m_entity_editor;
        RendererConfig*         m_renderer_config;
        MainEditorMenu*         m_menu;
        RenderableEditor*       m_renderable_editor;
        ProjectManager*         m_project_mgr;

        bool                    m_has_init;

public:
        bool builder_load(string filename);
        void builder_all_set();
};

EditorGtkFrontend::EditorBackendGtkFrontendInt::EditorBackendGtkFrontendInt(EditorGtkFrontend* frontend)
{
        m_splash_screen         = nullptr;
        m_main_editor           = nullptr;
        m_entity_editor         = nullptr;
        m_renderer_config       = nullptr;
        m_menu                  = nullptr;
        m_renderable_editor     = nullptr;
        m_project_mgr           = nullptr;

        m_has_init      = false;
        m_builder       = nullptr;
        m_editor        = nullptr;
        m_env           = nullptr;
}

EditorGtkFrontend::EditorBackendGtkFrontendInt::~EditorBackendGtkFrontendInt()
{
        m_splash_screen         = nullptr;
        m_main_editor           = nullptr;
        m_entity_editor         = nullptr;
        m_renderer_config       = nullptr;
        m_menu                  = nullptr;
        m_renderable_editor     = nullptr;
        m_project_mgr           = nullptr;

        g_object_unref(G_OBJECT(m_builder));

        m_has_init      = false;
        m_builder       = nullptr;
        m_editor        = nullptr;
        m_env           = nullptr;
}

bool EditorGtkFrontend::EditorBackendGtkFrontendInt::builder_load(string filename)
{
        if (m_builder == nullptr && !(m_builder = gtk_builder_new())) {
                log_severe_err_dbg("cannot create gtk-glade builder");
                return false;
        }
        GError* error = nullptr;
        if (!(gtk_builder_add_from_file(m_builder, filename.c_str(), &error))) {
                if (error) {
                        log_severe_err_dbg("builder fail to load: %s\n%s",
                                           filename.c_str(), error->message);
                        g_free(error);
                } else {
                        log_severe_err_dbg("builder fail to load: %s, unknown error",
                                           filename.c_str());
                }
                return false;
        }
        return true;
}

void EditorGtkFrontend::EditorBackendGtkFrontendInt::builder_all_set()
{
        gtk_builder_connect_signals(m_builder, nullptr);
}

EditorGtkFrontend::EditorGtkFrontend()
{
        pimpl = new EditorGtkFrontend::EditorBackendGtkFrontendInt(this);
}

EditorGtkFrontend::~EditorGtkFrontend()
{
        delete pimpl;
}

bool EditorGtkFrontend::is_usable()
{
        return pimpl->m_has_init;
}

void EditorGtkFrontend::close()
{
        log_normal_dbg("closing up gtk frontend");
        pimpl->m_editor->close();
}

EditorBackend* EditorGtkFrontend::get_core_editor()
{
        return pimpl->m_editor;
}

KernelEnvironment* EditorGtkFrontend::get_kernel_environment()
{
        return pimpl->m_env;
}

EditorGtkFrontend::EditorBackendMode EditorGtkFrontend::get_editor_mode()
{
        return pimpl->m_editor_mode;
}

GtkBuilder* EditorGtkFrontend::get_gtk_builder()
{
        return pimpl->m_builder;
}

MainEditor* EditorGtkFrontend::get_main_editor()
{
        return pimpl->m_main_editor;
}

MainEditorMenu* EditorGtkFrontend::get_main_editor_menu()
{
        return pimpl->m_menu;
}

RendererConfig* EditorGtkFrontend::get_renderer_config()
{
        return pimpl->m_renderer_config;
}

EntityEditor* EditorGtkFrontend::get_entity_editor()
{
        return pimpl->m_entity_editor;
}

static bool has_command(int argc, char** argv, string command)
{
        int i;
        for (i = 0; i < argc; i ++) {
                if (!strcmp(argv[i], command.c_str())) {
                        return true;
                }
        }
        return false;
}

bool EditorGtkFrontend::init(int argc, char **argv, EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_has_init = true;
        pimpl->m_editor = editor;
        pimpl->m_env = env;

        gtk_init(&argc, &argv);
        gdk_threads_init();

        pimpl->m_splash_screen         = new SplashScreen(this);
        pimpl->m_main_editor           = new MainEditor(this);
        pimpl->m_entity_editor         = new EntityEditor(this);
        pimpl->m_renderer_config       = new RendererConfig(this);
        pimpl->m_menu                  = new MainEditorMenu(this);
        pimpl->m_renderable_editor     = new RenderableEditor(this);
        pimpl->m_project_mgr           = new ProjectManager(this);

        if (!pimpl->m_splash_screen->show(true)) {
                log_mild_err_dbg ( "couldn't show splash screen" );
                return false;
        }

        /* determine the running mode from command line argument */
        if (has_command(argc, argv, "--edit-mode")) {
                pimpl->m_editor_mode = EditorGtkFrontend::EditMode;
        } else {
                pimpl->m_editor_mode = EditorGtkFrontend::DemoMode;
        }
        /* load in all glade files */
        const int c_NumGladeFiles = 6;
        string ls_dir[c_NumGladeFiles] = {
                m_glade_dir + m_main_editor,
                m_glade_dir + m_demo_player,
                m_glade_dir + m_renderer_conf,
                m_glade_dir + m_renderable_prop,
                m_glade_dir + m_entity_prop,
                m_glade_dir + m_entity_tree};
        int i;
        for (i = 0; i < c_NumGladeFiles; i ++) {
                if (!pimpl->builder_load(ls_dir[i])) {
                        log_severe_err("couldn't load in glade file: %s", ls_dir[i].c_str());
                        return false;
                }
        }
        pimpl->builder_all_set();
        return true;
}

bool EditorGtkFrontend::end_init(EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_editor = editor;
        pimpl->m_env = env;
        return true;
}

bool EditorGtkFrontend::load(EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_editor = editor;
        pimpl->m_env = env;

        if (!pimpl->m_main_editor->show(false)) {
                log_severe_err_dbg("couldn't load gtk main editor");
                return false;
        }
        if (!pimpl->m_renderer_config->show(true)) {
                log_severe_err_dbg("couldn't load gtk renderer configurator");
                return false;
        }
        if (!pimpl->m_entity_editor->show(true)) {
                log_severe_err_dbg("couldn't load gtk entity editor");
                return false;
        }
        if (!pimpl->m_renderable_editor->show(true)) {
                log_severe_err_dbg ( "couldn't load gtk renderable editor" );
                return false;
        }
        if (!pimpl->m_menu->show(true)) {
                log_severe_err_dbg("couldn't load gtk main editor menu");
                return false;
        }
        if (!pimpl->m_project_mgr->show(true)) {
                log_severe_err_dbg("couldn't load gtk project manager");
                return false;
        }
        return true;
}

void EditorGtkFrontend::loop ( EditorBackend *editor, KernelEnvironment *env )
{
        pimpl->m_editor = editor;
        pimpl->m_env = env;

        if (!pimpl->m_main_editor->show(true)) {
                log_severe_err_dbg("failed to show gtk main editor");
                return ;
        }
        if (!pimpl->m_splash_screen->show(false)) {
                log_mild_err_dbg("couldn't shutdown the splash screen");
                return ;
        }
        await_gtk_main();
}

bool EditorGtkFrontend::free(EditorBackend* editor, KernelEnvironment* env)
{
        delete pimpl->m_main_editor;
        delete pimpl->m_splash_screen;
        delete pimpl->m_entity_editor;
        delete pimpl->m_renderer_config;
        delete pimpl->m_menu;
        delete pimpl->m_renderable_editor;
        delete pimpl->m_project_mgr;
        return true;
}

/* @todo (davis#1#): implement file chooser */
string file_chooser_open ( string default_dir )
{
        return nullptr;
}

string file_chooser_save ( string default_dir )
{
        return nullptr;
}

void message_box_info ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void message_box_warning ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
}

void message_box_error ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW(dialog), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gtk_dialog_run ( GTK_DIALOG(dialog) );
        gtk_widget_destroy ( dialog );
}

bool message_box_question ( string title, string message )
{
        GtkWidget *dialog = nullptr;
        dialog = gtk_message_dialog_new ( nullptr, GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_QUESTION, GTK_BUTTONS_CLOSE,
                                          "%s", message.c_str () );
        gtk_window_set_position ( GTK_WINDOW(dialog), GTK_WIN_POS_CENTER );
        gtk_window_set_title ( GTK_WINDOW(dialog), title.c_str () );
        gint result = gtk_dialog_run ( GTK_DIALOG(dialog) );
        gtk_widget_destroy ( dialog );
        switch ( result ) {
        default:
        case GTK_RESPONSE_DELETE_EVENT:
        case GTK_RESPONSE_NO: {
                return false;
        }
        case GTK_RESPONSE_YES: {
                return true;
        }
        }
}

void widget_get_size ( GtkWidget* parent, GtkWidget* widget,
                       int* x, int* y, int* width, int* height )
{
        GtkAllocation *allocation = g_new0 ( GtkAllocation, 1 );
        gtk_widget_get_allocation ( widget, allocation );
        *width = allocation->width;
        *height = allocation->height;
        g_free ( allocation );
        gtk_widget_translate_coordinates ( widget, parent,
                                           0, 0, x, y );
}

static volatile int             g_gtk_main_stack = 0;
static x3d::thread_trap            g_atomic_check = {0};
static x3d::thread_task*           g_gtk_main_task = nullptr;

static void* do_gtk_main ( void* data )
{
        {
                gdk_threads_enter ();
                gtk_main ();
                gdk_threads_leave ();
        }
        return nullptr;
}

#define push_gtk_main()         (g_gtk_main_stack ++)
#define pop_gtk_main()          (g_gtk_main_stack --)

void stop_gtk_main ( void )
{
        x3d::thread_trap_on_task ( &g_atomic_check );
        if (g_gtk_main_stack == 0) {
                log_normal_dbg("nothing to stop");
                x3d::thread_untrap_task ( &g_atomic_check );
                return ;
        }
        pop_gtk_main ();
        if ( g_gtk_main_stack == 0 ) {
                log_normal_dbg ( "no gtk main is running: %d", g_gtk_main_stack );
                {
                        gdk_threads_enter ();
                        gtk_main_quit ();
                        gdk_threads_leave ();
                        x3d::thread_sync_with_task ( g_gtk_main_task );
                }
        } else {
                log_normal_dbg ( "found %d gtk main running already",
                                 g_gtk_main_stack );
        }
        x3d::thread_untrap_task ( &g_atomic_check );
}

void run_gtk_main ( void )
{
        x3d::thread_trap_on_task(&g_atomic_check);
        if ( g_gtk_main_stack > 0 ) {
                log_normal_dbg("found %d gtk main running already",
                                g_gtk_main_stack);
        } else {
                g_gtk_main_task = thread_run_task(get_function_name(), do_gtk_main, nullptr, nullptr );
                log_normal_dbg("starting gtk main");
        }
        push_gtk_main();
        x3d::thread_untrap_task(&g_atomic_check);
}

void await_gtk_main ( void )
{
        while ( g_gtk_main_stack != 0 );
        x3d::thread_trap_on_task ( &g_atomic_check );
        ; // do nothing
        x3d::thread_untrap_task ( &g_atomic_check );
}

void* dbg_get_render_region ( void )
{
        return main_editor_get_region ();
}

void dbg_hand_image_to_display ( void *ptr, int w, int h )
{
        /*        g_comm_data.tmp_image = ptr;
                g_comm_data.tmp_image_w = w;
                g_comm_data.tmp_image_h = h;*/
//        main_editor_draw_tmp_image ();
}

}// namespace usr

}// namespace x3d
