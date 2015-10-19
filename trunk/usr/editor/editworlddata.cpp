#include <usr/usr_editor.hpp>


namespace x3d
{
namespace usr
{

class WorldDataActiveX::WorldDataInt
{
public:
        WorldDataInt();
        ~WorldDataInt();

        bool    has_works();
/* @todo (davis#2#): <WorldDataActiveX> */
        bool            m_is_activated;
        World           m_world;
};

WorldDataActiveX::WorldDataInt::WorldDataInt()
{
        m_is_activated = false;
}

bool WorldDataActiveX::WorldDataInt::has_works()
{
        return (m_is_activated = m_world.get_render_aggregate()->get_instance_count() != 0);
}

WorldDataActiveX::WorldDataActiveX(string name) :
        EditorBackendActiveX ( name, sizeof(WorldDataActiveX), EDIT_ACTIVEX_WORLD_DATA )
{
        pimpl = new WorldDataActiveX::WorldDataInt;
}

WorldDataActiveX::~WorldDataActiveX()
{
}

void WorldDataActiveX::on_adding()
{
        // put itself to kernel environment
        KernelEnvironment* state = get_state_buffer();
        state->declare(c_WorldData, this);
}

void WorldDataActiveX::preupdate()
{
        pimpl->has_works();
}

void WorldDataActiveX::update()
{
        KernelEnvironment* state    = get_state_buffer();
//        RenderConfigActiveX* config = static_cast<RenderConfigActiveX*>(state->use(c_RenderConfig));
//        if (config) {
//                Renderer* renderer  = config->get_renderer();
//                pimpl->m_world.bind_render_processor(renderer);
//        }

        if (pimpl->m_is_activated) {
                pimpl->m_world.update();
        }
}

bool WorldDataActiveX::has_works()
{
        return pimpl->m_is_activated;
}

void WorldDataActiveX::dispatch()
{
}

void WorldDataActiveX::load(struct serializer *s)
{
}

void WorldDataActiveX::save(struct serializer *s)
{
}

World& WorldDataActiveX::get_world()
{
        return pimpl->m_world;
}

}// namespace usr

}// namespace x3d
