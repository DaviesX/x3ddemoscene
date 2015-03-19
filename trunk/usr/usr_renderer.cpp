#include <usr/usr_renderer.hpp>

namespace x3d
{
namespace usr
{

/** \brief initialize a renderer.
 *
 * Note that: once the type of the renderer is attached(it will be attached on its creation), it cannot easily be changed. a call of renderer_retype() can change the type of the renderer, but all the settings formerly applied to that renderer will be lost.
 * \param driver Driver the type of renderer to be created.
 */
Renderer::Renderer(Driver driver)
{
        renderer_init(&m_renderer, (RENDERER_IDR) driver);
}

/** \brief destruct the renderer.
 * \return void
 */
Renderer::~Renderer()
{
        renderer_free(&m_renderer);
}

/** \brief update rendering information from RenderTree.
 *
 * \param tree RenderTree* render input.
 * \return void
 *
 */
void Renderer::update(RenderTree* tree)
{
        renderer_render_tree(tree->get_core_resource(), &m_renderer);
        renderer_update(&m_renderer);
}

/** \brief Run the renderer.
 *
 * \return void
 *
 */
void Renderer::render()
{
        renderer_render(&m_renderer);
}

/** \brief commit the rendering result.
 *
 * \return void
 *
 */
void Renderer::commit()
{
        renderer_commit(&m_renderer);
}


RenderTree::RenderTree()
{
        render_tree_init(&m_rendertree);
}

RenderTree::~RenderTree()
{
        render_tree_free(&m_rendertree);
}

struct render_tree* RenderTree::get_core_resource()
{
        return &m_rendertree;
}

}// namespace usr

}// namespace x3d
