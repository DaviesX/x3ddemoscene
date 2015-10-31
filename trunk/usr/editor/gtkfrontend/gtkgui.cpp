#include <string>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include <gtk-3.0/gdk/gdkthreads.h>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

// properties exchanges
class EditorGtkFrontend::EditorGtkFrontendInt
{
public:
        EditorGtkFrontendInt(EditorGtkFrontend* frontend);
        ~EditorGtkFrontendInt();

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
        bool builder_load(std::string filename);
        void builder_all_set();
};

EditorGtkFrontend::EditorGtkFrontendInt::EditorGtkFrontendInt(EditorGtkFrontend* frontend)
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

EditorGtkFrontend::EditorGtkFrontendInt::~EditorGtkFrontendInt()
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

bool EditorGtkFrontend::EditorGtkFrontendInt::builder_load(std::string filename)
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
                        //g_free(error);
                } else {
                        log_severe_err_dbg("builder fail to load: %s, unknown error",
                                           filename.c_str());
                }
                return false;
        }
        return true;
}

void EditorGtkFrontend::EditorGtkFrontendInt::builder_all_set()
{
        gtk_builder_connect_signals(m_builder, nullptr);
}

EditorGtkFrontend::EditorGtkFrontend()
{
        pimpl = new EditorGtkFrontend::EditorGtkFrontendInt(this);
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
        
        // Notify gtk_main to shutdown
        stop_gtk_main();
        // Notify the backend to shutdown.
        // This may trigger the shutdown of the kernel as well.
        pimpl->m_editor->close();
}

EditorBackend* EditorGtkFrontend::get_backend_editor()
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

static bool has_command(int argc, char** argv, std::string command)
{
        int i;
        for (i = 0; i < argc; i ++) {
                if (!strcmp(argv[i], command.c_str())) {
                        return true;
                }
        }
        return false;
}

struct gtk_idle_packet {
        SplashScreen*           splash_screen = nullptr;
        MainEditor*             main_editor = nullptr;
        EntityEditor*           entity_editor = nullptr;  
        RendererConfig*         renderer_config = nullptr;
        MainEditorMenu*         main_editor_menu = nullptr;
        RenderableEditor*       renderable_editor = nullptr;
        ProjectManager*         project_manager = nullptr;
        bool                    has_finished = false;
        bool                    is_failed = false;
};

gboolean __gtk_init_do_tasks(struct gtk_idle_packet* user_data)
{
        if (!user_data->splash_screen->show(true)) {
                log_mild_err_dbg("couldn't show splash screen");
                user_data->is_failed = true;
                goto L1;
        }
L1:
        user_data->has_finished = true;
        return false;
}

struct __data {
        EditorGtkFrontend::EditorGtkFrontendInt*        pimpl = nullptr;
        std::string*                                    paths = nullptr; 
        int                                             n_num_paths = 0;
        bool                                            has_finished = false;
        bool                                            is_failed = false;
};

gboolean __gtk_init_do_tasks_load_glade_file(struct __data* user_data)
{
        int i;
        for (i = 0; i < user_data->n_num_paths; i ++) {
                if (!user_data->pimpl->builder_load(user_data->paths[i])) {
                        log_severe_err("couldn't load in glade file: %s", user_data->paths[i].c_str());
                        user_data->is_failed = true;
                        goto L1;
                }
        }
L1:      
        user_data->pimpl->builder_all_set();
        user_data->has_finished = true;
        return false;
}

bool EditorGtkFrontend::init(int argc, char **argv, EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_has_init = true;
        pimpl->m_editor = editor;
        pimpl->m_env = env;

        gtk_init(&argc, &argv);

        pimpl->m_splash_screen         = new SplashScreen(this);
        pimpl->m_main_editor           = new MainEditor(this);
        pimpl->m_entity_editor         = new EntityEditor(this);
        pimpl->m_renderer_config       = new RendererConfig(this);
        pimpl->m_menu                  = new MainEditorMenu(this);
        pimpl->m_renderable_editor     = new RenderableEditor(this);
        pimpl->m_project_mgr           = new ProjectManager(this);

        // launch gtk_main on the other thread, everything else should only be added to gtk_main's idle event.
        // since GTK isn't thread safe, only one thread is allowed.
        // use g_idle_add to add idle event to other threads
        run_gtk_main();

        // launch splash screen
        struct gtk_idle_packet p;
        {
                p.splash_screen        = pimpl->m_splash_screen;   
        }
        g_idle_add((GSourceFunc) __gtk_init_do_tasks, &p);
        while (!p.has_finished) thread_task_idle(10);
        if (p.is_failed)        return false;

        /* determine the running mode from command line argument */
        if (has_command(argc, argv, "--edit-mode")) {
                pimpl->m_editor_mode = EditorGtkFrontend::EditMode;
        } else {
                pimpl->m_editor_mode = EditorGtkFrontend::DemoMode;
        }
        /* load in all glade files */
        const int c_NumGladeFiles = 6;
        std::string ls_dir[c_NumGladeFiles] = {
                m_glade_dir + m_main_editor,
                m_glade_dir + m_demo_player,
                m_glade_dir + m_renderer_conf,
                m_glade_dir + m_renderable_prop,
                m_glade_dir + m_entity_prop,
                m_glade_dir + m_entity_tree};
        struct __data data;
        {
                data.n_num_paths        = c_NumGladeFiles;
                data.paths              = ls_dir;
                data.pimpl              = pimpl;
        }
        g_idle_add((GSourceFunc) __gtk_init_do_tasks_load_glade_file, &data);
        while (!data.has_finished)      thread_task_idle(10);
        if (data.is_failed)             return false;
        return true;
}

bool EditorGtkFrontend::end_init(EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_editor = editor;
        pimpl->m_env = env;
        return true;
}

gboolean __gtk_load_do_tasks(struct gtk_idle_packet* user_data)
{
        if (!user_data->main_editor->show(false)) {
                log_severe_err_dbg("couldn't load gtk main editor");
                goto L2;
        }
        if (!user_data->renderer_config->show(true)) {
                log_severe_err_dbg("couldn't load gtk renderer configurator");
                goto L2;
        }
        if (!user_data->entity_editor->show(true)) {
                log_severe_err_dbg("couldn't load gtk entity editor");
                goto L2;
        }
        if (!user_data->renderable_editor->show(true)) {
                log_severe_err_dbg("couldn't load gtk renderable editor");
                goto L2;
        }
        if (!user_data->main_editor_menu->show(true)) {
                log_severe_err_dbg("couldn't load gtk main editor menu");
                goto L2;
        }
        if (!user_data->project_manager->show(true)) {
                log_severe_err_dbg("couldn't load gtk project manager");
                goto L2;
        }
        goto L1;
L2:
        user_data->is_failed = true;
L1:
        user_data->has_finished = true;
        return false;
}

bool EditorGtkFrontend::load(EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_editor = editor;
        pimpl->m_env = env;
        
        struct gtk_idle_packet p;
        {
                p.entity_editor        = pimpl->m_entity_editor;
                p.main_editor          = pimpl->m_main_editor;
                p.main_editor_menu     = pimpl->m_menu;
                p.project_manager      = pimpl->m_project_mgr;
                p.renderer_config      = pimpl->m_renderer_config;
                p.renderable_editor    = pimpl->m_renderable_editor;
                p.splash_screen        = pimpl->m_splash_screen;
        }
        g_idle_add((GSourceFunc) __gtk_load_do_tasks, &p);
        while (!p.has_finished) thread_task_idle(10);
        if (p.is_failed)        return false;
        return true;
}

gboolean __gtk_loop_do_tasks(struct gtk_idle_packet* user_data)
{
        if (!user_data->main_editor->show(true)) {
                log_severe_err_dbg("failed to show gtk main editor");
                goto L2;
        }
        if (!user_data->splash_screen->show(false)) {
                log_mild_err_dbg("couldn't shutdown the splash screen");
                goto L2;
        }
        goto L1;
L2:
        user_data->is_failed = true;
L1:
        user_data->has_finished = true;
        return false;
}

void EditorGtkFrontend::loop(EditorBackend *editor, KernelEnvironment *env)
{
        pimpl->m_editor = editor;
        pimpl->m_env = env;

        struct gtk_idle_packet p;
        {
                p.main_editor          = pimpl->m_main_editor;
                p.splash_screen        = pimpl->m_splash_screen;
        }
        g_idle_add((GSourceFunc) __gtk_loop_do_tasks, &p);
        while (!p.has_finished) thread_task_idle(10);
        if (p.is_failed) {
                close();
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
std::string file_chooser_open(std::string default_dir)
{
        return nullptr;
}

std::string file_chooser_save(std::string default_dir)
{
        return nullptr;
}

static gint __show_message_box(std::string title, std::string message, GtkMessageType type, GtkWindow* parent)
{
        GtkDialogFlags flags;
        GtkButtonsType buttons;
        if (type == GTK_MESSAGE_QUESTION) {
                flags = GTK_DIALOG_MODAL;
                buttons = GTK_BUTTONS_YES_NO;
        } else {
                flags = GTK_DIALOG_DESTROY_WITH_PARENT;
                buttons = GTK_BUTTONS_OK;
        }
        GtkWidget* dialog = gtk_message_dialog_new(parent, flags,
                                                   type, buttons,
                                                   "%s", message.c_str());
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
        gtk_window_set_modal(GTK_WINDOW(dialog), false);
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        gtk_widget_show_all(dialog);
        if (type == GTK_MESSAGE_QUESTION) {
                gint ans = gtk_dialog_run(GTK_DIALOG (dialog));
                gtk_widget_destroy(dialog);
                return ans;
        } else {
                g_signal_connect_swapped(dialog, "response",
                                         G_CALLBACK (gtk_widget_destroy),
                                         dialog);
                return 0;
        }
}

void message_box_info(std::string title, std::string message, GtkWindow* parent)
{
        __show_message_box(title, message, GTK_MESSAGE_INFO, parent);
}

void message_box_warning(std::string title, std::string message, GtkWindow* parent)
{
        __show_message_box(title, message, GTK_MESSAGE_WARNING, parent);
}

void message_box_error(std::string title, std::string message, GtkWindow* parent)
{
        __show_message_box(title, message, GTK_MESSAGE_ERROR, parent);
}

bool message_box_question(std::string title, std::string message, GtkWindow* parent)
{
        switch(__show_message_box(title, message, GTK_MESSAGE_QUESTION, parent)) {
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

void widget_get_size(GtkWidget* parent, GtkWidget* widget,
                       int* x, int* y, int* width, int* height)
{
        GtkAllocation *allocation = g_new0(GtkAllocation, 1);
        gtk_widget_get_allocation(widget, allocation);
        *width                  = allocation->width;
        *height                 = allocation->height;
        g_free(allocation);
        gtk_widget_translate_coordinates(widget, parent,
                                           0, 0, x, y);
}

static volatile int             g_gtk_main_stack = 0;
static x3d::thread_trap         g_atomic_check = {0};
static x3d::thread_task*        g_gtk_main_task = nullptr;

static void* do_gtk_main(void* data)
{
        gtk_main ();
        log_normal("gtk main exited");
        return nullptr;
}

#define push_gtk_main()         (g_gtk_main_stack ++)
#define pop_gtk_main()          (g_gtk_main_stack --)

// This function does not guarantee the exit of gtk main on its return
void stop_gtk_main()
{
        x3d::thread_trap_on_task(&g_atomic_check);
        if (g_gtk_main_stack == 0) {
                log_normal("nothing to stop");
                x3d::thread_untrap_task(&g_atomic_check);
                return ;
        }
        pop_gtk_main ();
        if(g_gtk_main_stack == 0) {
                log_normal("no gtk main is running: %d", g_gtk_main_stack);
                {
                        gtk_main_quit ();
//                        x3d::thread_sync_with_task(g_gtk_main_task);
                }
        } else {
                log_normal("found %d gtk main running already",
                                 g_gtk_main_stack);
        }
        x3d::thread_untrap_task(&g_atomic_check);
}

// This function does not guarantee the presence of gtk main on its return
void run_gtk_main()
{
        x3d::thread_trap_on_task(&g_atomic_check);
        if(g_gtk_main_stack > 0) {
                log_normal_dbg("found %d gtk main running already",
                                g_gtk_main_stack);
        } else {
                g_gtk_main_task = thread_run_task(get_function_name(), do_gtk_main, nullptr, nullptr);
                log_normal_dbg("starting gtk main");
        }
        push_gtk_main();
        x3d::thread_untrap_task(&g_atomic_check);
}

// This function will block the thread until the gtk main exited
void await_gtk_main()
{
        x3d::thread_sync_with_task(g_gtk_main_task);
        
//        while (g_gtk_main_stack != 0) { thread_task_idle(10); }
//        x3d::thread_trap_on_task(&g_atomic_check);
//        ; // do nothing
//        x3d::thread_untrap_task(&g_atomic_check);
}

void* dbg_get_render_region()
{
        return main_editor_get_region ();
}

void dbg_hand_image_to_display(void *ptr, int w, int h)
{
        /*        g_comm_data.tmp_image = ptr;
                g_comm_data.tmp_image_w = w;
                g_comm_data.tmp_image_h = h;*/
//        main_editor_draw_tmp_image ();
}

}// namespace usr

}// namespace x3d
