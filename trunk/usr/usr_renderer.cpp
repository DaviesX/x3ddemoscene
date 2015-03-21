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

RenderNode* RenderOutput::create(string name, string probe)
{
        return (RenderNode*) (m_node = (struct render_output*) render_node_output_create(name.c_str(), probe.c_str()));
}

void RenderOutput::set_probe(string probe)
{
        render_node_output_set_probe(m_node, probe.c_str());
}

RenderNode* RenderLayer::create(string name)
{
        return (RenderNode*) (m_node = (struct render_layer*) render_node_layer_create(name.c_str()));
}

RenderNode* RenderableContext::create(string name, string context, RenderAggregate::Strategy strategy)
{
        return (RenderNode*) (m_node = (struct render_rdacontext*)
                              render_node_rdacontext_create(name.c_str(), context.c_str(), (RAG_IDR) strategy));
}

void RenderableContext::set_context(string context)
{
        render_node_rdacontext_set_context(m_node, context.c_str());
}

void RenderableContext::set_strategy(RenderAggregate::Strategy strategy)
{
        render_node_rdacontext_set_access(m_node, (RAG_IDR) strategy);
}

RenderNode* RenderRadiance::create(string name, RenderPipeline pipe)
{
        return (RenderNode*) (m_node = (struct render_radiance*)
                              render_node_radiance_create(name.c_str(), (RENDER_PIPE_IDR) pipe));
}

void RenderRadiance::set_pipeline(RenderPipeline pipe)
{
        render_node_radiance_set_pipe(m_node, (RENDER_PIPE_IDR) pipe);
}

struct packer {
        void*                   user_data;
        RenderTreeVisitor*      this_ptr;
};

static void push_node (struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct packer* packer = (struct packer*) dataptr;
        packer->this_ptr->push((RenderNode*) node, packer->user_data);
}

static void bactrack_node (struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct packer* packer = (struct packer*) dataptr;
        packer->this_ptr->backtrack((RenderNode*) node, packer->user_data);
}

RenderTreeVisitor::RenderTreeVisitor(void* data)
{
        struct packer* packer = (struct packer*) alloc_obj(packer);
        packer->this_ptr = this;
        packer->user_data = data;
        m_data = (void*) packer;
        render_tree_visitor_init2(&m_visitor, push_node, push_node, m_data);
}

struct render_tree_visitor* RenderTreeVisitor::get_core_resource()
{
        return &m_visitor;
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

RenderNode* RenderTree::get_node(string name)
{
        return (RenderNode*) render_tree_get_node(&m_rendertree, name.c_str());
}

RenderNode* RenderTree::create_root()
{
        return (RenderNode*) render_tree_create_root(&m_rendertree);
}

bool RenderTree::insert_node(RenderNode* parent, RenderNode* node)
{
        return render_tree_insert_node(&m_rendertree, (struct render_node*) parent, (struct render_node*) node);
}

bool RenderTree::replace_node(RenderNode* node)
{
        return render_tree_replace_node(&m_rendertree, (struct render_node*) node);
}

bool RenderTree::remove_node(RenderNode* parent, RenderNode* node)
{
        return render_tree_remove_node(&m_rendertree, (struct render_node*) parent, (struct render_node*) node);
}

void RenderTree::set_environment_variable(EnvironmentType type, string var_name, void* var)
{
        render_tree_declare_environment(&m_rendertree, (RENDER_ENVIRONMENT) type, var_name.c_str(), var);
}

bool RenderTree::visit(RenderTreeVisitor* visitor)
{
        return render_tree_visit(&m_rendertree, visitor->get_core_resource());
}


}// namespace usr

}// namespace x3d
