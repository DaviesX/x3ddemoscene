#include <usr/usr_world.hpp>

namespace x3d
{
namespace usr
{

World::World() :
        m_renderagg(RenderAggregate::SimpleLinear)
{
        m_resloader = nullptr;
        m_renderer = nullptr;
}

World::~World()
{
}

RenderAggregate* World::get_render_aggregate()
{
        return &m_renderagg;
}

ResourceLoader* World::get_resource_processor()
{
        return m_resloader;
}

Renderer* World::get_render_processor()
{
        return m_renderer;
}

void World::bind_resource_processor(ResourceLoader* loader)
{
        m_resloader = loader;
}

void World::bind_render_processor(Renderer* renderer)
{
        m_renderer = renderer;
}

void World::update()
{
        if (m_renderer) {
                m_renderer->render();
        }
}

}// namespace usr

}// namespace x3d

