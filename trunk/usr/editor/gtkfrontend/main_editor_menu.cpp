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
        GtkMenu*                        m_benchmenu;
        GtkMenuItem*                    m_benchitem;

};


extern "C" void cornell_box_callback ( GtkMenuItem* menuitem, gpointer user_data )
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(user_data);
        BenchmarkActiveX* benchmark = static_cast<BenchmarkActiveX*>
                (frontend->get_core_editor()->find_activex( EditorActiveX::EDIT_ACTIVEX_BENCHMARK, "benchmark-scene-menu-item"));
        benchmark->run_benchmark(BenchmarkActiveX::Benchmark_CornellBox);
}

extern "C" void trigger_quit_callback(GtkWidget *widget, gpointer data)
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(data);
        frontend->close();
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

        // deal with the benchmark menu
        {
        pimpl->m_benchmenu = (GtkMenu*) gtk_builder_get_object(builder, "benchmark-scene");
        if (!pimpl->m_benchmenu) {
                log_severe_err_dbg("cannot retrieve benchmark-scene menu");
                return false;
        }
        pimpl->m_benchitem = (GtkMenuItem*) gtk_builder_get_object(builder, "benchmark-scene-menu-item");
        if (!pimpl->m_benchitem) {
                log_severe_err_dbg("cannot retrieve benchmark-scene-menu-item widget");
                return false;
        }
        // cornell box benchmark
        GtkMenuItem* cornell_box_item = (GtkMenuItem*) gtk_builder_get_object(builder, "cornell-box-scene");
        if (!cornell_box_item) {
                log_severe_err_dbg ( "cannot retrieve cornell-box-scene menu item" );
                return false;
        }
        g_signal_connect(G_OBJECT(cornell_box_item), "activate",
                         G_CALLBACK(cornell_box_callback), frontend);
        BenchmarkActiveX* benchmark = new BenchmarkActiveX("benchmark-scene-menu-item");
        frontend->get_core_editor()->add_activex(benchmark);
        }
        // deal with the close menu
        {
        GtkImageMenuItem* quit_menu = (GtkImageMenuItem*) gtk_builder_get_object(builder, "quit-menu-item");
        if (!quit_menu) {
                log_severe_err_dbg("cannot retrieve quit-menu-item widget");
                return false;
        }
        g_signal_connect(G_OBJECT(quit_menu), "activate",
                         G_CALLBACK(trigger_quit_callback), frontend);
        }

        if (visible) {
                // perform injection to benchmark menu
                gtk_menu_item_set_submenu(pimpl->m_benchitem, GTK_WIDGET(pimpl->m_benchmenu));
        } else {
                gtk_menu_item_set_submenu(nullptr, GTK_WIDGET(pimpl->m_benchmenu));
        }
        return true;
}


}// namespace usr

}// namespace x3d
