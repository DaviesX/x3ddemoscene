#include <usr/usr_editor.hpp>

using namespace x3d;
using namespace x3d::usr;


class WorldDataActiveX::WorldDataInt
{
public:
/* @todo (davis#2#): <WorldDataActiveX> */
};


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
        struct x3d::rda_context* context =
                (struct x3d::rda_context*) state->use ( c_World );
        if ( context ) {
                x3d::rda_context_free ( context );
        }
        context = x3d::rda_context_create ( RAG_LINEAR );
        state->declare ( c_World, context );
}

void WorldDataActiveX::update ( void )
{
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
