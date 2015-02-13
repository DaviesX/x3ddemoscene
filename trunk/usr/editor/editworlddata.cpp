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


WorldDataActiveX::WorldDataActiveX ( string name ) :
        EditorActiveX ( name, sizeof(WorldDataActiveX), EDIT_ACTIVEX_WORLD_DATA )
{
        pimpl = new WorldDataActiveX::WorldDataInt;
}

WorldDataActiveX::~WorldDataActiveX ( void )
{
}

void WorldDataActiveX::on_adding ( void )
{
        // make a default renderable context
        KernelEnvironment* state = this->get_state_buffer ();
        /*
        struct x3d::rda_context* context =
                (struct x3d::rda_context*) state->use ( c_WorldData );
        if ( context ) {
                x3d::rda_context_free ( context );
        }
        context = x3d::rda_context_create ( RAG_LINEAR );*/
        state->declare ( c_WorldData, this );
}

void WorldDataActiveX::update ( void )
{
        pimpl->m_world.update();
}

void WorldDataActiveX::dispatch ( void )
{
}

void WorldDataActiveX::load ( struct serializer *s )
{
}

void WorldDataActiveX::save ( struct serializer *s )
{
}

World& WorldDataActiveX::get_world()
{
        return pimpl->m_world;
}

}// namespace usr

}// namespace x3d
