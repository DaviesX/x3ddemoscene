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
        bool init ( int argc, char **argv,
                    Editor *editor, KernelEnvironment *env );
        bool end_init ( Editor *editor, KernelEnvironment *env );
        bool load ( Editor *editor, KernelEnvironment *env );
        void loop ( Editor *editor, KernelEnvironment *env );
        bool free ( Editor *editor, KernelEnvironment *env );
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
