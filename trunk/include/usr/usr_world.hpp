#ifndef USR_WORLD_HPP_INCLUDED
#define USR_WORLD_HPP_INCLUDED

#include <usr/usr_renderaggregate.hpp>
#include <usr/usr_resourceloader.hpp>
#include <usr/usr_renderer.hpp>

namespace x3d
{
namespace usr
{

/** \brief World (helper class).
 */
class World {
public:
        World();
        ~World();

        RenderAggregate* get_render_aggregate();
        ResourceLoader* get_resource_processor();
        Renderer* get_render_processor();

        void bind_resource_processor(ResourceLoader* loader);
        void bind_render_processor(Renderer* renderer);
	void detach_render_processor();
        void update();

private:
        RenderAggregate         m_renderagg;
        ResourceLoader*         m_resloader;
        Renderer*               m_renderer;
};

}// namespace usr

}// namespace x3d

#endif // USR_WORLD_HPP_INCLUDED
