#ifndef MAIN_WINDOW_MENU_HPP_INCLUDED
#define MAIN_WINDOW_MENU_HPP_INCLUDED

namespace x3d
{
namespace usr
{

class MainEditorMenu
{
public:
        MainEditorMenu(EditorGtkFrontend* frontend);
        ~MainEditorMenu();

        bool show(bool visible);
private:
        class MainEditorMenuInt;
        class MainEditorMenuInt* pimpl;
};

}// namespace usr

}// namespace x3d

#endif // MAIN_WINDOW_MENU_HPP_INCLUDED
