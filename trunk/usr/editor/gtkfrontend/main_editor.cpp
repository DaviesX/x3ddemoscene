/* main_editor.cpp: implementation of the main editor control */
#include <gtk/gtk.h>
#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_editorfrontend.hpp>
#include "gtkgui.hpp"

namespace x3d
{
namespace usr
{

class MainEditor::MainEditorInt {
public:
        MainEditorInt(EditorGtkFrontend* frontend);
        ~MainEditorInt();
public:
        EditorGtkFrontend*                      m_frontend;
        bool                                    m_has_loaded;
        GtkBuilder*                             m_builder;
        GtkWidget*                              m_window;
        GtkWidget*                              m_draw_region;
        GdkPixbuf*                              m_logo;
        GdkPixbuf*                              m_icon;
        struct project_mgr*     proj_mgr;
        struct entity_mgr*      ent_mgr;
        struct entity_prop*     ent_prop;
        struct assist_dialog*   assist_diag;

        gulong                                  m_logo_draw_signal;
        gulong                                  m_idle_dispatch;
        gulong                                  m_destroy_signal;
        bool                                    m_is_logo_connected;
};

static const string cRenderRegion = "gtk-main-editor-render-region";

/*
static struct main_editor g_main_edit;
*/
extern "C" gboolean destroy_callback(GtkWidget *widget, gpointer data);
extern "C" gboolean main_editor_dispatch(gpointer user_data);

static void idle_switch_callback(bool is_idle, EditorActiveX* ax, void *info);
static void window_reize_callback(bool is_fullscreen, int width, int height,
                                  EditorActiveX* ax, void *info);
static void render_config_error_callback(string message, RenderConfigActiveX *conf, void *data);
extern "C" gboolean display_logo_callback(GtkWidget *draw_region, cairo_t *cairo, gpointer data);


extern "C" gboolean display_logo_callback(GtkWidget *draw_region, cairo_t *cairo, gpointer data)
{
        MainEditor::MainEditorInt* pimpl = static_cast<MainEditor::MainEditorInt*>(data);
        int x, y, w, h;
        widget_get_size(pimpl->m_window, pimpl->m_draw_region, &x, &y, &w, &h);
        GdkPixbuf* scaled = gdk_pixbuf_scale_simple(pimpl->m_logo, w, h, GDK_INTERP_NEAREST);
        gdk_cairo_set_source_pixbuf(cairo, scaled, 0, 0);
        gdk_pixbuf_unref(scaled);
        cairo_paint(cairo);
        cairo_fill(cairo);
        return 0;
}

static void idle_switch_callback(bool is_idle, EditorActiveX* ax, void *info)
{
        MainEditor::MainEditorInt* pimpl = static_cast<MainEditor::MainEditorInt*>(info);
        GtkWidget* draw_area = pimpl->m_draw_region;
        if (is_idle) {
                if (!pimpl->m_is_logo_connected) {
                        pimpl->m_logo_draw_signal =
                                g_signal_connect(draw_area, "draw", G_CALLBACK(display_logo_callback), pimpl);
                        gtk_widget_queue_draw(pimpl->m_draw_region);
                        pimpl->m_is_logo_connected = true;
                }
        } else {
                if (pimpl->m_is_logo_connected) {
                        g_signal_handler_disconnect(draw_area, pimpl->m_logo_draw_signal);
                        GdkColor color;
                        color.red   = 0X0;
                        color.blue  = 0X0;
                        color.green = 0X0;
                        gtk_widget_modify_bg(pimpl->m_draw_region, GTK_STATE_NORMAL, &color);
                        pimpl->m_is_logo_connected = false;
                }
        }
}

static void window_reize_callback(bool is_fullscreen, int width, int height, EditorActiveX* ax, void *info)
{
}

extern "C" gboolean destroy_callback(GtkWidget *widget, gpointer data)
{
        MainEditor::MainEditorInt* pimpl = static_cast<MainEditor::MainEditorInt*>(data);
        pimpl->m_frontend->close();
        return 0;
}

MainEditor::MainEditorInt::MainEditorInt(EditorGtkFrontend* frontend)
{
        m_frontend = frontend;
        m_has_loaded = false;
        m_is_logo_connected = false;
}

MainEditor::MainEditorInt::~MainEditorInt()
{
        stop_gtk_main();
        await_gtk_main();

        gdk_pixbuf_unref(m_logo);
        gdk_pixbuf_unref(m_icon);
        g_signal_handler_disconnect(m_draw_region, m_logo_draw_signal);
        g_signal_handler_disconnect(m_window, m_idle_dispatch);
        g_signal_handler_disconnect(m_window, m_destroy_signal);

        if (m_frontend->is_usable()) {
                m_frontend->get_core_editor()->remove_activex(EditorActiveX::EDIT_ACTIVEX_RENDER_REGION, cRenderRegion);
        }
}

MainEditor::MainEditor(EditorGtkFrontend* frontend)
{
        pimpl = new MainEditorInt(frontend);
}

MainEditor::~MainEditor()
{
        delete pimpl;
}

bool MainEditor::show(bool visible)
{
        if (!pimpl->m_has_loaded) {
                /* set current control window */
                EditorGtkFrontend* frontend = pimpl->m_frontend;
                string file = frontend->m_glade_dir;
                GtkBuilder *builder = frontend->get_gtk_builder();
                if (!builder)
                        return false;

                if (frontend->get_editor_mode() == EditorGtkFrontend::DemoMode) {
demo_mode:
                        // connect to demo window
                        pimpl->m_window = (GtkWidget*) gtk_builder_get_object(builder, "demo-window");
                        if (!pimpl->m_window) {
                                log_severe_err_dbg ( "cannot retrieve demo window" );
                                return false;
                        }
                        pimpl->m_draw_region = (GtkWidget*) gtk_builder_get_object(builder, "demo-render-region");
                        if (!pimpl->m_draw_region) {
                                log_severe_err_dbg("cannot retrieve demo render region widget");
                                return false;
                        }
                } else if (frontend->get_editor_mode() == EditorGtkFrontend::EditMode) {
                        // connect to editor window
                        pimpl->m_window = (GtkWidget*) gtk_builder_get_object(builder, "main-editor-window");
                        if (!pimpl->m_window) {
                                log_severe_err_dbg("cannot retrieve main-editor window");
                                return false;
                        }
                        pimpl->m_draw_region = (GtkWidget*) gtk_builder_get_object(builder, "DA-RenderRegion");
                        if (!pimpl->m_draw_region) {
                                log_severe_err_dbg("cannot retrieve draw-area widget");
                                return false;
                        }
                } else {
                        log_mild_err_dbg("mode is not specified corrected. selected mode: %d. running into demo mode...",
                                         frontend->get_editor_mode());
                        goto demo_mode;
                }
                // DON'T trash it yet.
                /* done using glade builder */
                // builder_all_set ( builder );
                pimpl->m_builder = builder;
                gtk_builder_connect_signals(pimpl->m_builder, nullptr);

                /* change background color to black */
                GdkColor color;
                color.red   = 0X0;
                color.blue  = 0X0;
                color.green = 0X0;
                gtk_widget_modify_bg(pimpl->m_draw_region, GTK_STATE_NORMAL, &color);

                /* load in logo file */
                GError *error = nullptr;
                string logo_file = frontend->m_glade_dir + "x3d_logo.png";
                pimpl->m_logo = gdk_pixbuf_new_from_file(logo_file.c_str(), &error);
                if (error != nullptr) {
                        log_severe_err_dbg("couldn't load logo file: %s", logo_file.c_str());
                        log_severe_err_dbg("gtk internal message: %s", error->message);
                        g_free(error);
                        return false;
                }
                string icon_file = frontend->m_glade_dir + "icon.png";
                pimpl->m_icon = gdk_pixbuf_new_from_file(icon_file.c_str(), &error);
                if (error != nullptr) {
                        log_severe_err_dbg("couldn't load icon file: %s", icon_file.c_str());
                        log_severe_err_dbg("gtk internal message: %s", error->message);
                        g_free(error);
                        return false;
                }
                /* set the size of the window */
                static const float c_WindowRatio = 0.9f;
                static const float c_Pane1Ratio = 6.7f/8.0f*c_WindowRatio;
                static const float c_Pane2Ratio = 1.3f/8.0f*c_WindowRatio;
                static const float c_Pane3Ratio = 2.5f/6.0f*c_WindowRatio;
                int win_width, win_height;
                GdkScreen* screen = gdk_screen_get_default();
                win_width  = gdk_screen_get_width(screen);
                win_height = gdk_screen_get_height(screen);
                gtk_window_set_icon(GTK_WINDOW(pimpl->m_window), pimpl->m_icon);
                gtk_window_resize(GTK_WINDOW(pimpl->m_window), win_width*c_WindowRatio, win_height*c_WindowRatio);
                /* set the size of the pane */
                GtkPaned* pane1 = (GtkPaned*) gtk_builder_get_object(builder, "main-editor-pane1");
                GtkPaned* pane2 = (GtkPaned*) gtk_builder_get_object(builder, "main-editor-pane2");
                GtkPaned* pane3 = (GtkPaned*) gtk_builder_get_object(builder, "main-editor-pane3");
                if (!pane1 || !pane2 || !pane3) {
                        log_severe_err_dbg("cannot retrieve main editor pane widget");
                        return false;
                }
                gtk_paned_set_position(pane1, win_width*c_Pane1Ratio);
                gtk_paned_set_position(pane2, win_width*c_Pane2Ratio);
                gtk_paned_set_position(pane3, win_height*c_Pane3Ratio);
                /* create render region */
                int x, y;
                int width, height;
                widget_get_size(pimpl->m_window, pimpl->m_draw_region, &x, &y, &width, &height);
                RenderRegionActiveX* ax_region = new RenderRegionActiveX(cRenderRegion, GtkRenderRegionOutput,
                                                                         pimpl->m_draw_region,
                                                                         x, y, width, height );
                ax_region->bind_callback("notify_idle", (f_Generic) idle_switch_callback, pimpl);
                ax_region->bind_callback("notify_resize", (f_Generic) window_reize_callback, pimpl);
                frontend->get_core_editor()->add_activex(ax_region);

                /* set dispatch function which fetches signal
                 * from core editor when gtk_main is idle
                 * also, connect the window destroy signal */
                pimpl->m_idle_dispatch = g_idle_add(main_editor_dispatch, pimpl);
                pimpl->m_destroy_signal =
                        g_signal_connect(G_OBJECT(pimpl->m_window), "destroy", G_CALLBACK(destroy_callback), pimpl);

                pimpl->m_has_loaded = true;
        }

        gdk_threads_enter();
        /* show window */
        if (visible) {
                gtk_widget_show_all(pimpl->m_window);
                run_gtk_main();
        } else {
                gtk_widget_hide(pimpl->m_window);
        }
        gdk_threads_leave();
        return true;
}

GtkWidget* main_editor_get_region ( void )
{
        return nullptr;
}

extern "C" gboolean main_editor_dispatch(gpointer user_data)
{
        MainEditor::MainEditorInt* pimpl = static_cast<MainEditor::MainEditorInt*>(user_data);
        pimpl->m_frontend->get_core_editor()->dispatch_signal();
        return true;
}


} // namespace usr

} // namespace x3d
