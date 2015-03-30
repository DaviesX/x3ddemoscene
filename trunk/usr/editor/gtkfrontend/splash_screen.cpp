#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

class SplashScreen::SplashScreenInt
{
public:
        SplashScreenInt(EditorGtkFrontend* frontend);
        ~SplashScreenInt();
public:
        bool                    m_has_loaded;
        EditorGtkFrontend*      m_frontend;
        GtkWidget*              m_image;
        GtkWidget*              m_window;
};

SplashScreen::SplashScreenInt::SplashScreenInt(EditorGtkFrontend* frontend)
{
        m_has_loaded    = false;
        m_frontend      = frontend;
        m_image         = nullptr;
        m_window        = nullptr;
}

SplashScreen::SplashScreenInt::~SplashScreenInt()
{
        m_has_loaded    = false;
        m_frontend      = nullptr;
        m_image         = nullptr;
        m_window        = nullptr;
}

SplashScreen::SplashScreen(EditorGtkFrontend* frontend)
{
        pimpl = new SplashScreenInt(frontend);
}

SplashScreen::~SplashScreen()
{
        delete pimpl;
}

bool SplashScreen::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;

        if (!pimpl->m_has_loaded) {
                string filename = frontend->m_glade_dir + frontend->m_logo;

                pimpl->m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
                gtk_widget_set_size_request(pimpl->m_window, 800, 600);
                gtk_window_set_decorated(GTK_WINDOW(pimpl->m_window), FALSE);
                gtk_window_set_position(GTK_WINDOW(pimpl->m_window), GTK_WIN_POS_CENTER_ALWAYS);
                gtk_window_set_resizable(GTK_WINDOW(pimpl->m_window), FALSE);
                if ( !(pimpl->m_image = gtk_image_new_from_file(filename.c_str()))) {
                        log_severe_err_dbg("couldn't load logo file from %s", filename.c_str());
                        return false;
                }
                gtk_container_add(GTK_CONTAINER(pimpl->m_window), pimpl->m_image);
                pimpl->m_has_loaded = true;
        }
        if (visible) {
                gtk_widget_show_all(pimpl->m_window);
                run_gtk_main();
        } else {
                gdk_threads_enter();
                gtk_widget_destroy (pimpl->m_image);
                gtk_widget_destroy(pimpl->m_window);
                gdk_threads_leave();
                stop_gtk_main();
        }
        return true;
}

}// namespace usr

}// namespace x3d
