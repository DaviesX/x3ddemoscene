/*! \file usr/usr_renderaggregate.cpp
 *  \brief class RenderAggregate implementation
 */
#include <usr/usr_renderable.hpp>
#include <usr/usr_renderaggregate.hpp>
#include <usr/usr_x3d.hpp>


namespace x3d
{
namespace usr
{

 /** \brief Construct a render aggregate by chosen strategy.
  *
  * \param strategy Strategy maintenance strategy for the aggregate
  *
  */
RenderAggregate::RenderAggregate(Strategy strategy)
{
        RenderAggregateType rag;
        switch (strategy) {
        case RenderAggregate::SimpleLinear:
                rag = RenderAggregateLinear;
                break;
        case RenderAggregate::StaticBvh:
                rag = RenderAggregateStaticBVH;
                break;
        case RenderAggregate::DynamicGrid:
                rag = RenderAggregateDynamicGrid;
                break;
        default:
                rag = RenderAggregateLinear;
                log_normal("Unknown strategy, falls back to SimpleLinear");
                break;
        }
        rda_context_init(&m_context, rag);
}

/** @brief Deconstruct created resources.
  */
 RenderAggregate::~RenderAggregate()
{
        rda_context_free(&m_context);
}

/** @brief get the number of renderable inside the aggregate.
 *
 * \return int the number of renderable.
  */
int RenderAggregate::get_renderable_count()
{
        return rda_context_get_renderable_count(&m_context);
}

/** @brief whether the instance exists within current context.
  *
  * It doesn't indicate the inherent existance of the instance. But it represents the activeness associated with the internal container.
  */
bool RenderAggregate::has_instance(RenderableInstance* instance)
{
        return rda_context_has_instance(&m_context, instance->get_core_resource());
}

/** \brief remove the instance from aggregate.
 *
 * \param instance RenderableInstance* the instance to be removed
 * \return void
 *
 */
void RenderAggregate::remove_instance(RenderableInstance* instance)
{
        rda_context_remove_instance(&m_context, instance->get_core_resource());
}

/** \brief add the instance to aggregate.
 *
 * \param instance RenderableInstance* instance to be added
 * \return void
 *
 */
void RenderAggregate::add_instance(RenderableInstance* instance)
{
        rda_context_add_instance2(&m_context, instance->get_core_resource());
}

/** \brief remove the renderable from aggregate.
 *
 * \param name string name of the renderable to be removed.
 * \return void
 *
 */
void RenderAggregate::remove_renderable(string name)
{
        rda_context_remove_renderable_by_name(&m_context, const_cast<char*>(name.c_str()));
}

/** \brief remove the renderable from aggregate.
 *
 * \param renderable Renderable* instance to be removed.
 * \return void
 *
 */
void RenderAggregate::remove_renderable(Renderable* renderable)
{
        rda_context_remove_renderable(&m_context, renderable->get_core_resource());
}

/** \brief add the renderable to aggregate.
 *
 * \param renderable Renderable* renderable to be added.
 * \return void
 *
 */
void RenderAggregate::add_renderable(Renderable* renderable)
{
        rda_context_add_renderable(&m_context, renderable->get_core_resource());
}

/** \brief clear everything from aggregate.
 *
 * Both renderables and instances will be cleared from the aggregate.
 * \return void
 *
 */
void RenderAggregate::clear()
{
        rda_context_clear(&m_context);
}

/** \brief get the number of instance in the aggregate.
 *
 * \return int the number of instance.
 *
 */
int RenderAggregate::get_instance_count()
{
        return rda_context_get_instance_count(&m_context);
}

/** \brief find the renderable by name.
 *
 * \param name string name of the renderable to be found.
 * \return Renderable* found renderable, or nullptr if it doesn't exist.
 *
 */
Renderable* RenderAggregate::find_renderable(string name)
{
        return new Renderable(rda_context_find_renderable_by_name(&m_context, const_cast<char*>(name.c_str())));
}

/** \brief test whether the renderable exists.
 *
 * \param renderable Renderable* the renderable to be tested.
 * \return bool whether it exists.
 *
 */
bool RenderAggregate::has_renderable(Renderable* renderable)
{
        return rda_context_has_renderable(&m_context, renderable->get_core_resource());
}

struct rda_context* RenderAggregate::get_core_resource()
{
        return &m_context;
}

}// namespace usr

}// namespace x3d
