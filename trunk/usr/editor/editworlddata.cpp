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
        World          m_world;
};

WorldDataActiveX::WorldDataInt::WorldDataInt()
{
}


WorldDataActiveX::WorldDataActiveX(string name) :
        EditorActiveX ( name, sizeof(WorldDataActiveX), EDIT_ACTIVEX_WORLD_DATA )
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

void WorldDataActiveX::update()
{
        KernelEnvironment* state    = get_state_buffer();
        RenderConfigActiveX* config = static_cast<RenderConfigActiveX*>(state->use(c_RenderConfig));
        if (config) {
                Renderer* renderer  = config->get_renderer();
                pimpl->m_world.bind_render_processor(renderer);
        }

        pimpl->m_world.update();
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
