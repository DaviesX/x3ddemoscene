#ifndef USR_RENDERER_HPP_INCLUDED
#define USR_RENDERER_HPP_INCLUDED

#include <usr/usr_x3d.hpp>

namespace x3d
{
namespace usr
{

class __dlexport RenderTree : public CoreResource {
public:
        RenderTree ();
        ~RenderTree ();

        struct render_tree* get_core_resource();
private:
        struct render_tree      m_rendertree;
};

/** \brief Render processsor.
 */
class Renderer : public CoreResource {
public:
        enum Driver {
                RENDERER_UNDETERMINATE,         /**< a memory block with undefined renderer */
                RENDERER_RASTERIZER,            /**< rasterization renderer */
                RENDERER_PATH_TRACER,           /**< path tracing renderer */
                RENDERER_PHOTON_TRACER,         /**< photon tracing renderer */
                RENDERER_PHOTON_MAP,            /**< photon light map generation renderer */
                RENDERER_RADIOSITY,             /**< radiosity light map generation renderer */
                RENDERER_RADIANCE_CACHE,        /**< radiance cache generation renderer */
                RENDERER_PRT,                   /**< precomputed radiance transfer renderer */
                RENDERER_SELECTION              /**< object selection renderer */
        };

        Renderer(Driver driver);
        ~Renderer();

        void update(RenderTree* tree);
        void render();
        void commit();

        struct renderer* get_core_resource();
private:
        struct renderer         m_renderer;
};

}// namespace usr

}// namespace x3d


#endif // USR_RENDERER_HPP_INCLUDED
