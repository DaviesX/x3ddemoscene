#ifndef OBJECT_EDITOR_H_INCLUDED
#define OBJECT_EDITOR_H_INCLUDED


/*
 * functions' declaration
 */
namespace x3d
{
namespace usr
{

class EntityEditor {
public:
        EntityEditor(EditorGtkFrontend* frontend);
        ~EntityEditor();

        bool    show(bool visible);
private:
        class EntityEditorInt;
        class EntityEditorInt* pimpl;
};

}// namespace usr

}// namespace x3d

#endif // OBJECT_EDITOR_H_INCLUDED
