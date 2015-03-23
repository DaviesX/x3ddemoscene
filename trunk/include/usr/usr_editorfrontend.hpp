#ifndef EDITORBACKEND_HPP_INCLUDED
#define EDITORBACKEND_HPP_INCLUDED


#include <usr/usr_editor.hpp>

namespace x3d
{

namespace usr
{

/*
 * Classes' declarations
 */
class EditorGtkFrontend : public EditorFrontend
{
public:
        enum EditorMode {
                DemoMode,
                EditMode
        };

        EditorGtkFrontend();
        ~EditorGtkFrontend();

        bool init ( int argc, char **argv,
                    Editor *editor, KernelEnvironment *env );
        bool end_init ( Editor *editor, KernelEnvironment *env );
        bool load ( Editor *editor, KernelEnvironment *env );
        void loop ( Editor *editor, KernelEnvironment *env );
        bool free ( Editor *editor, KernelEnvironment *env );

public:
        const string            m_glade_dir = "./etc/editor/";
        const string            m_media_dir = "./etc/media/";
        const string            m_config_dir = "./etc/config/";

        const string            m_logo          = "x3d_logo2.png";
        const string            m_main_editor   = "main_editor_window.glade";
        const string            m_demo_player   = "demo_player.glade";
        const string            m_splash_screen = "splash_screen_window.glade";
        const string            m_renderable_prop = "renderable_property_widget.glade";
        const string            m_entity_prop   = "entity_property_widget.glade";
public:
        bool                    is_usable();
        void                    close();
        Editor*                 get_core_editor();
        KernelEnvironment*      get_kernel_environment();
        EditorMode              get_editor_mode();
private:
        bool splash_screen_load ( void );
        bool splash_screen_show ( bool is_visible );
        bool splash_screen_shut ( void );

        bool render_config_load ( void );
        bool render_config_show ( bool is_visible );
        bool render_config_shut ( void );

        bool main_editor_menu_load ( void );
        bool main_editor_menu_show ( bool is_visible );
        bool main_editor_menu_shut ( void );

        bool project_manager_load ( void );
        bool project_manager_show ( bool is_visible );
        bool project_manager_shut ( void );
private:
        class EditorGtkFrontendInt;
        class EditorGtkFrontendInt*     pimpl;
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

bool editor_init ( int *argc, char ***argv );
bool editor_end_init ( void );
bool editor_load ( void );
void editor_loop ( void *info );
bool editor_free ( void );

void *dbg_get_render_region ( void );
void dbg_hand_image_to_display ( void *ptr, int w, int h );

}

}


#endif // EDITORBACKEND_HPP_INCLUDED
