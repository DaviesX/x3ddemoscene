#include <usr/usr_world.hpp>

namespace x3d
{
namespace usr
{

World::World() :
        m_renderagg(RenderAggregate::SimpleLinear)
{
}

World::~World()
{
}

RenderAggregate* World::get_render_aggregate()
{
        return &m_renderagg;
}

ResourceLoader* World::get_resource_processor(uuid_t id)
{
        if (id == m_resloaderid) {
                return m_resloader;
        } else {
                return nullptr;
        }
}

Renderer* World::get_render_processor(uuid_t id)
{
        if (id == m_rendererid) {
                return m_renderer;
        } else {
                return nullptr;
        }
}

uuid_t World::bind_resource_processor(ResourceLoader* loader)
{
        m_resloader     = loader;
        m_resloaderid   = alg_gen_uuid();
        return m_resloaderid;
}

uuid_t World::bind_render_processor(Renderer* renderer)
{
        m_renderer      = renderer;
        m_rendererid    = alg_gen_uuid();
        return m_rendererid;
}

bool World::detach_render_processor(uuid_t id)
{
        if (id == m_rendererid) {
                m_renderer = nullptr;
                return true;
        } else {
                return false;
        }
}

void World::update()
{
        if (m_renderer) {
                m_renderer->render();
        }
}

}// namespace usr

}// namespace x3d

