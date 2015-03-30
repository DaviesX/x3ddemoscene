#ifndef RENDERER_CONFIGURATOR_HPP_INCLUDED
#define RENDERER_CONFIGURATOR_HPP_INCLUDED

namespace x3d
{
namespace usr
{

class RendererConfig
{
public:
        RendererConfig(EditorGtkFrontend* frontend);
        ~RendererConfig();
        bool show(bool visible);
private:
        class RendererConfigInt;
        class RendererConfigInt*        pimpl;
};

}// namespace usr

}// namespace x3d


#endif // RENDERER_CONFIGURATOR_HPP_INCLUDED
