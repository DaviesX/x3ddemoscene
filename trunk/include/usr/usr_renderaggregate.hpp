#ifndef USR_RENDERAGGREGATE_HPP_INCLUDED
#define USR_RENDERAGGREGATE_HPP_INCLUDED

#include <usr/usr_x3d.hpp>
#include <usr/usr_renderable.hpp>

namespace x3d
{
namespace usr
{
/** \brief Containers for renderable and its instances.
 */
class RenderAggregate : public CoreResource<struct rda_context> {
public:
        enum Strategy {
                SimpleLinear,
                StaticBvh,
                DynamicGrid
        };

        RenderAggregate ( Strategy strategy );
        ~RenderAggregate ();

        void add_instance ( RenderableInstance* instance );
        void remove_instance ( RenderableInstance* instance );
        bool has_instance ( RenderableInstance* instance );
        int get_instance_count();
        void clear();

        void add_renderable ( Renderable* renderable );
        void remove_renderable ( Renderable* renderable );
        void remove_renderable ( string name );
        bool has_renderable ( Renderable* renderable );
        Renderable* find_renderable ( string name );
        int get_renderable_count();

        struct rda_context* get_core_resource();
private:
        struct rda_context      m_context;
};

}// namespace usr

}// namespace x3d


#endif // USR_RENDERAGGREGATE_HPP_INCLUDED
