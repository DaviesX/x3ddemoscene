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

        RenderAggregate*        get_render_aggregate();
        ResourceLoader*         get_resource_processor(uuid_t id);
        Renderer*               get_render_processor(uuid_t id);

        uuid_t                  bind_resource_processor(ResourceLoader* loader);
        uuid_t                  bind_render_processor(Renderer* renderer);
	bool                    detach_render_processor(uuid_t id);
        void                    update();

private:
        RenderAggregate         m_renderagg;
        uuid_t                  m_renderaggid = -1;
        ResourceLoader*         m_resloader = nullptr;
        uuid_t                  m_resloaderid = -1;
        Renderer*               m_renderer = nullptr;
        uuid_t                  m_rendererid = -1;
};

}// namespace usr

}// namespace x3d

#endif // USR_WORLD_HPP_INCLUDED
