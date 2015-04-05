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
        stop_gtk_main();
        await_gtk_main();

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

struct timer_pack {
        volatile bool                   has_finished;
        int                             n;
        int                             interval;
        int                             length;
        float                           s_trans;
        SplashScreen::SplashScreenInt*  pimpl;
};

static gboolean splash_diminisher(gpointer data)
{
        struct timer_pack* pack = static_cast<struct timer_pack*>(data);
        float trans = pack->s_trans - pack->n ++*pack->interval*(pack->s_trans - 0.0f)/(float) pack->length;
        if (trans < 0.0f) {
                gtk_widget_destroy(pack->pimpl->m_image);
                gtk_widget_destroy(pack->pimpl->m_window);
                pack->has_finished = true;
                return FALSE;
        }
        gtk_widget_set_opacity(GTK_WIDGET(pack->pimpl->m_window), trans);
        return TRUE;
}

bool SplashScreen::show(bool visible)
{
        EditorGtkFrontend* frontend = pimpl->m_frontend;

        if (!pimpl->m_has_loaded) {
                string filename = frontend->m_glade_dir + frontend->m_logo;

                pimpl->m_window = gtk_window_new(GTK_WINDOW_POPUP);
                gtk_widget_set_size_request(pimpl->m_window, 800, 600);
                gtk_window_set_decorated(GTK_WINDOW(pimpl->m_window), TRUE);
                gtk_window_set_position(GTK_WINDOW(pimpl->m_window), GTK_WIN_POS_CENTER_ALWAYS);
                gtk_window_set_resizable(GTK_WINDOW(pimpl->m_window), FALSE);
                if ( !(pimpl->m_image = gtk_image_new_from_file(filename.c_str()))) {
                        log_severe_err_dbg("couldn't load logo file from %s", filename.c_str());
                        return false;
                }
                gtk_container_add(GTK_CONTAINER(pimpl->m_window), pimpl->m_image);
                gtk_widget_set_opacity(GTK_WIDGET(pimpl->m_window), 1.0f);
                pimpl->m_has_loaded = true;
        }
        if (visible) {
                gtk_widget_show_all(pimpl->m_window);
                run_gtk_main();
        } else {
                struct timer_pack* pack = (struct timer_pack*) alloc_obj(pack);
                pack->n          = 0;
                pack->interval   = 10;
                pack->length     = 2000;
                pack->s_trans    = 0.8;
                pack->pimpl      = pimpl;
                g_timeout_add(pack->interval, splash_diminisher, (gpointer) pack);
                while(!pack->has_finished);
                free_fix(pack);
                stop_gtk_main();
        }
        return true;
}

}// namespace usr

}// namespace x3d
