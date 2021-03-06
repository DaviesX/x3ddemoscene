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
Renderer::Renderer()
{
/* NOTE (davis#9#): renderer doesn't need init */
//        renderer_init(&m_renderer, (RENDERER_IDR) driver);
}

/** \brief destruct the renderer.
 * \return void
 */
Renderer::~Renderer()
{
/* NOTE (davis#9#): renderer doesn't need free */
        // renderer_free(&m_renderer);
}

/** \brief update rendering information from RenderTree.
 *
 * \param tree RenderTree* render input.
 * \return void
 *
 */
void Renderer::update(RenderTree* tree)
{
        // renderer_render_tree(tree->get_core_resource(), &m_renderer);
        // renderer_update(&m_renderer);
        m_tree = tree;
}

/** \brief Run the renderer.
 *
 * \return void
 *
 */
void Renderer::render()
{
        renderer_render(m_tree->get_core_resource());
}

/** \brief commit the rendering result.
 *
 * \return void
 *
 */
//void Renderer::commit()
//{
/* NOTE (davis#9#): renderer doesn't need commit */
        // renderer_commit(&m_renderer);
//}

/* NOTE (davis#9#): renderer no core resource */
//struct renderer* Renderer::get_core_resource()
//{
//        return &m_renderer;
//}


RenderOutput::RenderOutput(string name, string probe)
{
        m_node = (struct render_output*) render_node_output_create(name.c_str(), probe.c_str());
}

RenderNode* RenderOutput::get_node()
{
        return (RenderNode*) m_node;
}

void RenderOutput::set_probe(string probe)
{
        render_node_output_set_probe(m_node, probe.c_str());
}

RenderLayer::RenderLayer(string name)
{
        m_node = (struct render_layer*) render_node_layer_create(name.c_str());
}

RenderNode* RenderLayer::get_node()
{
        return (RenderNode*) m_node;
}

RenderableLoader::RenderableLoader(string name, string context, RenderAggregateType strategy)
{
        m_node = (struct render_rdaloader*) render_node_rdaloader_create(name.c_str(), context.c_str(), strategy);
}

RenderNode* RenderableLoader::get_node()
{
        return (RenderNode*) m_node;
}

void RenderableLoader::set_context(string context)
{
        render_node_rdaloader_set_context(m_node, context.c_str());
}

void RenderableLoader::set_strategy(RenderAggregateType strategy)
{
        render_node_rdaloader_set_access(m_node, strategy);
}

RenderRadiance::RenderRadiance(string name, RenderPipeType pipe)
{
        m_node = (struct render_radiance*) render_node_radiance_create(name.c_str(), pipe);
}

RenderNode* RenderRadiance::get_node()
{
        return (RenderNode*) m_node;
}

void RenderRadiance::set_pipeline(RenderPipeType pipe)
{
        render_node_radiance_set_pipe(m_node, pipe);
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

void RenderTree::set_environment_variable(RenderEnvironment type, string var_name, void* var)
{
        render_tree_declare_environment(&m_rendertree, type, var_name.c_str(), var);
}

void RenderTree::clear_environment_variables()
{
        render_tree_clear_environment(&m_rendertree);
}

bool RenderTree::visit(RenderTreeVisitor* visitor)
{
        return render_tree_visit(&m_rendertree, visitor->get_core_resource());
}


}// namespace usr

}// namespace x3d
