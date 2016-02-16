#ifndef RENDERABLE_EDITOR_H_INCLUDED
#define RENDERABLE_EDITOR_H_INCLUDED


/*
 * functions' declaration
 */
namespace x3d
{
namespace usr
{

class RenderableEditor
{
public:
        RenderableEditor(EditorGtkFrontend* frontend);
        ~RenderableEditor();
        bool show(bool visible);
public:
        class RenderableEditorInt;
        class RenderableEditorInt* pimpl;
};

}// namespace usr

}// namespace x3d

#endif // RENDERABLE_EDITOR_H_INCLUDED
