#include <usr/usr_editorfrontend.hpp>
#include <gtk/gtk.h>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{


extern "C" void cornell_box_callback ( GtkMenuItem* menuitem, gpointer user_data )
{
        Editor* editor = (Editor*) user_data;
        BenchmarkActiveX* benchmark =
                (BenchmarkActiveX*) editor->find_activex ( EditorActiveX::EDIT_ACTIVEX_BENCHMARK,
                                                           "benchmark-scene-menu-item" );
        benchmark->run_benchmark ( BenchmarkActiveX::Benchmark_CornellBox );
}

extern "C" void trigger_quit_callback(GtkWidget *widget, gpointer data)
{
        EditorGtkFrontend* frontend = static_cast<EditorGtkFrontend*>(data);
        frontend->close();
}

bool EditorGtkFrontend::main_editor_menu_load ( void )
{
        if (get_editor_mode() == EditorGtkFrontend::DemoMode)
                return true;

        string file = m_glade_dir + m_main_editor;
        GtkBuilder* builder = nullptr;
        if (!(builder = builder_load(file)))
                return false;
        // deal with the benchmark menu
        {
        GtkMenu* bench_menu = (GtkMenu*) gtk_builder_get_object ( builder, "benchmark-scene" );
        if ( !bench_menu ) {
                log_severe_err_dbg ( "cannot retrieve benchmark-scene menu" );
                return false;
        }
        GtkMenuItem* bench_item = (GtkMenuItem*) gtk_builder_get_object ( builder, "benchmark-scene-menu-item" );
        if ( !bench_item ) {
                log_severe_err_dbg ( "cannot retrieve benchmark-scene-menu-item widget" );
                return false;
        }
        gtk_menu_item_set_submenu ( bench_item, (GtkWidget*) bench_menu );
        // cornell box benchmark
        GtkMenuItem* cornell_box_item = (GtkMenuItem*) gtk_builder_get_object ( builder, "cornell-box-scene" );
        if ( !cornell_box_item ) {
                log_severe_err_dbg ( "cannot retrieve cornell-box-scene menu item" );
                return false;
        }
        g_signal_connect(G_OBJECT(cornell_box_item), "activate",
                         G_CALLBACK(cornell_box_callback), get_core_editor());
        BenchmarkActiveX* benchmark = new BenchmarkActiveX ( "benchmark-scene-menu-item" );
        get_core_editor()->add_activex(benchmark);
        }
        // deal with the close menu
        {
        GtkImageMenuItem* quit_menu = (GtkImageMenuItem*) gtk_builder_get_object ( builder, "quit-menu-item" );
        if ( !quit_menu ) {
                log_severe_err_dbg ( "cannot retrieve quit-menu-item widget" );
                return false;
        }
        g_signal_connect(G_OBJECT(quit_menu), "activate",
                         G_CALLBACK(trigger_quit_callback), this);
        }
        return true;
}

bool EditorGtkFrontend::main_editor_menu_show ( bool is_visible )
{
        return true;
}

bool EditorGtkFrontend::main_editor_menu_shut ( void )
{
        return true;
}


}// namespace usr

}// namespace x3d
