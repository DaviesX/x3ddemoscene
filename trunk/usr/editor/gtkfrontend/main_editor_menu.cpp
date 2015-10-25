#include <usr/usr_editorfrontend.hpp>
#include <gtk/gtk.h>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

class MainEditorMenu::MainEditorMenuInt
{
public:
        MainEditorMenuInt(EditorGtkFrontend* frontend);
        ~MainEditorMenuInt();
public:
        EditorGtkFrontend*              m_frontend;

};


extern "C" void gtk_cornell_box_callback(GtkMenuItem* menuitem, gpointer user_data)
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(user_data);
        BenchmarkActiveX* benchmark = static_cast<BenchmarkActiveX*>
                (frontend->get_backend_editor()->find_activex(EditorBackendActiveX::EditActiveXBenchmark, 
                                                              gtkactivex::c_BackendBenchmark));
        benchmark->run_benchmark(BenchmarkActiveX::Benchmark_CornellBox);
}

void x3d_render_frame_finish(std::string stats, RenderFrameActiveX& ax, void* user_data)
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(user_data);
        GtkWindow* parent = frontend->get_main_editor()->get_window_widget();
        message_box_info("X3d Render Frame", "Render frame has finished\nstatistics:" + stats, parent);
}

extern "C" void gtk_render_current_frame_callback(GtkMenuItem* menuitem, gpointer user_data)
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(user_data);
        RenderFrameActiveX* renderframe_ax = static_cast<RenderFrameActiveX*>
                (frontend->get_backend_editor()->find_activex(EditorBackendActiveX::EditActiveXRenderFrame, 
                                                              gtkactivex::c_BackendRenderFrame));
        renderframe_ax->run_frame_renderer(true);
        renderframe_ax->bind_callback("notify_finish", (f_Generic) x3d_render_frame_finish, frontend);
}

extern "C" void gtk_trigger_quit_callback(GtkWidget* widget, gpointer data)
{
        if (message_box_question("X3d demoscene", "Do you want to exit?", nullptr)) {
                EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(data);
                frontend->close();
        }
}

MainEditorMenu::MainEditorMenuInt::MainEditorMenuInt(EditorGtkFrontend* frontend)
{
        m_frontend = frontend;
}

MainEditorMenu::MainEditorMenuInt::~MainEditorMenuInt()
{
        stop_gtk_main();
        await_gtk_main();

        m_frontend = nullptr;
}

MainEditorMenu::MainEditorMenu(EditorGtkFrontend* frontend)
{
        pimpl = new MainEditorMenu::MainEditorMenuInt(frontend);
}

MainEditorMenu::~MainEditorMenu()
{
        delete pimpl;
}

bool MainEditorMenu::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;

        if (frontend->get_editor_mode() == EditorGtkFrontend::DemoMode)
                return true;

        GtkBuilder* builder = nullptr;
        if (!(builder = frontend->get_gtk_builder()))
                return false;

        // deal with the <benchmark> menu
        {
                GtkMenuItem* mi_load_benchmark = (GtkMenuItem*) gtk_builder_get_object(builder, "MI-LoadBenchmark");
                if (!mi_load_benchmark) {
                        log_severe_err("cannot retrieve MI-LoadBenchmark menu");
                        return false;
                }
                GtkMenu* m_benchmark = (GtkMenu*) gtk_builder_get_object(builder, "M-BenchmarkScene");
                if (!m_benchmark) {
                        log_severe_err("cannot retrieve M-BenchmarkScene widget");
                        return false;
                }
                // inject the benchmark menu to main menu
                gtk_menu_item_set_submenu(mi_load_benchmark, GTK_WIDGET(m_benchmark));
                
                // Cornell box benchmark
                GtkMenuItem* mi_cornell = (GtkMenuItem*) gtk_builder_get_object(builder, "MI-CornellBoxScene");
                if (!mi_cornell) {
                        log_severe_err("cannot retrieve MI-CornellBoxScene menu item");
                        return false;
                }
                g_signal_connect(G_OBJECT(mi_cornell), "activate",
                                 G_CALLBACK(gtk_cornell_box_callback), frontend);
        }
        // deal with the <Render Current Frame> menu
        {
                GtkMenuItem* mi_render_current = 
                        (GtkMenuItem*) gtk_builder_get_object(builder, "MI-RenderCurrentFrame");
                if (!mi_render_current) {
                        log_severe_err("cannot retrieve MI-RenderCurrentFrame menu");
                        return false;
                }
                g_signal_connect(G_OBJECT(mi_render_current), "activate",
                                 G_CALLBACK(gtk_render_current_frame_callback), frontend);
        }
        // deal with the <close> menu
        {
                GtkImageMenuItem* quit_menu = (GtkImageMenuItem*) gtk_builder_get_object(builder, "MI-Quit");
                if (!quit_menu) {
                        log_severe_err("cannot retrieve MI-Quit widget");
                        return false;
                }
                g_signal_connect(G_OBJECT(quit_menu), "activate",
                                 G_CALLBACK(gtk_trigger_quit_callback), frontend);
        }
        // deal with ActiveX
        BenchmarkActiveX* benchmark = new BenchmarkActiveX(gtkactivex::c_BackendBenchmark);
        if (!frontend->get_backend_editor()->add_activex(benchmark)) {
                delete benchmark;
        }
        RenderFrameActiveX* rendframe_ax = new RenderFrameActiveX(gtkactivex::c_BackendRenderFrame);
        if (!frontend->get_backend_editor()->add_activex(rendframe_ax)) {
                delete rendframe_ax;
        }
        
        return true;
}


}// namespace usr

}// namespace x3d
