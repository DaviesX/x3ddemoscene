#ifndef USR_RENDERER_HPP_INCLUDED
#define USR_RENDERER_HPP_INCLUDED

#include <usr/usr_x3d.hpp>
#include <usr/usr_renderaggregate.hpp>

namespace x3d
{
namespace usr
{

class RenderNode;

/** \brief Render output node
 */
class RenderOutput : public CoreResource<struct render_output> {
public:
        RenderOutput(string name, string probe);
        RenderNode*             get_node();
        void                    set_probe(string probe);
private:
        struct render_output*   m_node;
};

/** \brief Render layer node
 */
class RenderLayer : public CoreResource<struct render_layer> {
public:
        RenderLayer(string name);
        RenderNode*             get_node();
private:
        struct render_layer*    m_node;
};

/** \brief Renerable context node
 */
class RenderableContext : public CoreResource<struct render_rdacontext> {
public:
        RenderableContext(string name, string context, RenderAggregate::Strategy strategy);
        RenderNode*             get_node();
        void                    set_context(string context);
        void                    set_strategy(RenderAggregate::Strategy strategy);
private:
        struct render_rdaloader* m_node;
};

/** \brief Render radiance node
 */
class RenderRadiance : public CoreResource<struct render_radiance> {
public:
        enum RenderPipeline {
                Forward,
                ForwardPlus,
                Whitted,
                DirectLightning,
                Metropolis,
                ImageSpace
        };
        RenderRadiance(string name, RenderPipeline pipe);
        RenderNode*             get_node();
        void                    set_pipeline(RenderPipeline pipe);
private:
        struct render_radiance* m_node;
};

/** \brief Render tree visitor
 */
class RenderTreeVisitor {
public:
        RenderTreeVisitor(void* data);

        virtual void            push(RenderNode* node, void* dataptr)          = 0;
        virtual void            backtrack(RenderNode* node, void* dataptr)     = 0;

        struct render_tree_visitor*     get_core_resource();
private:
        struct render_tree_visitor      m_visitor;
        void*                           m_data;
};

/** \brief Render tree
 */
class __dlexport RenderTree : public CoreResource<struct render_tree> {
public:
        RenderTree ();
        ~RenderTree ();

        enum EnvironmentType {
                Void,
                Spec,
                Probe,
                RDA,
                Thread,
                Antialias,
                Filter
        };

        RenderNode*             get_node(string name);
        RenderNode*             create_root();
        bool                    insert_node(RenderNode* parent, RenderNode* node);
        bool                    replace_node(RenderNode* node);
        bool                    remove_node(RenderNode* parent, RenderNode* node);
        void                    set_environment_variable(EnvironmentType type, string var_name, void* var);
        void                    clear_environment_variables();
        bool                    visit(RenderTreeVisitor* visitor);

        struct render_tree*     get_core_resource();
private:
        struct render_tree      m_rendertree;
};

/** \brief Render processsor.
 */
class Renderer : public CoreResource<struct renderer> {
public:
        enum Driver {
                Undeterminate,          /**< a memory block with undefined renderer */
                Rasterizer,             /**< rasterization renderer */
                Pathtracer,             /**< path tracing renderer */
                Photontracer,           /**< photon tracing renderer */
                Photonmap,              /**< photon light map generation renderer */
                Radiosity,              /**< radiosity light map generation renderer */
                RadianceCache,          /**< radiance cache generation renderer */
                PRT,                    /**< precomputed radiance transfer renderer */
                Selection               /**< object selection renderer */
        };

        Renderer(Driver driver);
        ~Renderer();

        void                    update(RenderTree* tree);
        void                    render();
/* NOTE (davis#9#): renderer no core resource */
        // struct renderer*        get_core_resource();
private:
        RenderTree*             m_tree;
};

}// namespace usr

}// namespace x3d


#endif // USR_RENDERER_HPP_INCLUDED
