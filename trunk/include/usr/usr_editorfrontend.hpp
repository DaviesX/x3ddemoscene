#ifndef EDITORBACKEND_HPP_INCLUDED
#define EDITORBACKEND_HPP_INCLUDED


#include <usr/usr_editor.hpp>

typedef struct _GtkBuilder GtkBuilder;


namespace x3d
{

namespace usr
{

class MainEditor;
class MainEditorMenu;
class EntityEditor;
class RendererConfig;

namespace gtkactivex
{
static const string c_BackendMainRenderRegion   = "backend-render-region";
static const string c_BackendRenderFrame        = "backend-render-frame";
static const string c_BackendBenchmark          = "backend-benchmark";
static const string c_BackendPowersaveSwitch    = "backend-powersave-switch";
};

/*
 * Classes' declarations
 */
class EditorGtkFrontend : public EditorFrontend
{
public:
        enum EditorBackendMode {
                DemoMode,
                EditMode
        };

        EditorGtkFrontend();
        ~EditorGtkFrontend();

        bool init(int argc, char **argv, EditorBackend *editor, KernelEnvironment *env);
        bool end_init(EditorBackend *editor, KernelEnvironment *env);
        bool load(EditorBackend *editor, KernelEnvironment *env);
        void loop(EditorBackend *editor, KernelEnvironment *env);
        bool free(EditorBackend *editor, KernelEnvironment *env);

public:
        const string            m_glade_dir = "./etc/editor/";
        const string            m_media_dir = "./etc/media/";
        const string            m_config_dir = "./etc/config/";

        const string            m_logo          = "x3d_logo2.png";
        const string            m_main_editor   = "main_editor_window.glade";
        const string            m_demo_player   = "demo_player.glade";
        const string            m_renderer_conf = "renderer_configurator.glade";
        const string            m_renderable_prop = "renderable_property_widget.glade";
        const string            m_entity_prop   = "entity_property_widget.glade";
        const string            m_entity_tree   = "entity_tree_widget.glade";
public:
        bool                    is_usable();
        void                    close();
        EditorBackend*          get_backend_editor();
        KernelEnvironment*      get_kernel_environment();
        EditorBackendMode       get_editor_mode();
        GtkBuilder*             get_gtk_builder();
        MainEditor*             get_main_editor();
        MainEditorMenu*         get_main_editor_menu();
        RendererConfig*         get_renderer_config();
        EntityEditor*           get_entity_editor();
private:
        class EditorBackendGtkFrontendInt;
        class EditorBackendGtkFrontendInt*     pimpl;
};

class EditorQtFrontend : public EditorFrontend
{
};

class EditorWin32Frontend : public EditorFrontend
{
};

class EditorPureSdlFrontend : public EditorFrontend
{
};

bool editor_init(int *argc, char ***argv);
bool editor_end_init(void);
bool editor_load(void);
void editor_loop(void *info);
bool editor_free(void);

void *dbg_get_render_region(void);
void dbg_hand_image_to_display(void *ptr, int w, int h);

}

}


#endif // EDITORBACKEND_HPP_INCLUDED
