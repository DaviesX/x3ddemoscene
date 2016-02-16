#ifndef MAIN_EDITOR_H_INCLUDED
#define MAIN_EDITOR_H_INCLUDED


#include <usr/usr_editorfrontend.hpp>

/*
 * functions' declaration
 */
namespace x3d
{
namespace usr
{

GtkWidget *main_editor_get_region ( void );
void main_editor_draw_tmp_image ( void );

class MainEditor {
public:
        MainEditor(EditorGtkFrontend* frontend);
        ~MainEditor();

        bool                    show(bool visible);
        void                    signal_quit_event();
        void                    set_window_title(std::string title);
        
        enum MessageType {
                MessageInfo,
                MessageWarning,
                MessageError,
                MessageQuestion
        };
        bool                    send_message_box(std::string title, std::string content, MessageType msg_type);

        class MainEditorInt;
        class MainEditorInt*    pimpl;
};

}// namespace usr

}// namespce x3d


#endif // MAIN_EDITOR_H_INCLUDED
