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
/* @todo (davis#2#): <WorldDataActiveX> */
        bool            m_is_activated;
        World           m_world;
};

WorldDataActiveX::WorldDataInt::WorldDataInt()
{
        m_is_activated = false;
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
        pimpl->m_is_activated = true;
}

void WorldDataActiveX::update()
{
        KernelEnvironment* state    = get_state_buffer();
        RenderConfigActiveX* config = static_cast<RenderConfigActiveX*>(state->use(c_RenderConfig));
        if (config) {
                Renderer* renderer  = config->get_renderer();
                pimpl->m_world.bind_render_processor(renderer);
        }

        if (pimpl->m_is_activated) {
                pimpl->m_world.update();
        }
}

void WorldDataActiveX::deactivate()
{
        pimpl->m_is_activated = false;
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
