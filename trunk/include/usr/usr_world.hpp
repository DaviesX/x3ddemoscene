#ifndef USR_WORLD_HPP_INCLUDED
#define USR_WORLD_HPP_INCLUDED

#include <usr_renderaggregate.hpp>

namespace x3d
{
namespace usr
{

class World {
public:
/*        enum Node {
                RenderableAggregateNode,
                DynamicAggregateNode,
                PointOfInterestNode,
                ResourceLoaderNode
        };

        enum Processor {
                RenderProcessor,
                DynamicsProessor,
                ResourceProcessor
        };*/

        World ();
        ~World ();

        RenderAggregate& get_render_aggregate ();
        ResourceLoader* get_resource_processor ();
        Renderer* get_render_processor ();

        void bind_resource_processor ( ResourceLoader* loader );
        void bind_render_processor ( Renderer* renderer );
        void update ();

private:
        RenderAggregate         m_renderagg;
        ResourceLoader*         m_resloader;
        Renderer*               m_renderer;
};

}
}

#endif // USR_WORLD_HPP_INCLUDED
