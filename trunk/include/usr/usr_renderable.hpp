#ifndef USR_RENDERABLE_HPP_INCLUDED
#define USR_RENDERABLE_HPP_INCLUDED

#include <usr/usr_x3d.hpp>

namespace x3d
{
namespace usr
{

class Renderable;
class RenderableInstance;


/** \brief Renderable Factory.
 */
class RenderableFactory
{
public:
        enum RenderableDesc {
                Geometry
        };

        Renderable* create ( RenderableDesc desc, string name, bool movable );
};

/** \brief Renderable base class.
 */
class Renderable : public CoreResource
{
public:
        Renderable(struct renderable* renderable);

        void                    set_importance ( float importance );
        void                    set_material ( int mater_ref );
        string                  get_name ();
        RenderableFactory::RenderableDesc get_desc ();
        RenderableInstance*     make_instance ( struct matrix4x4* transform );
        struct renderable*      get_core_resource();
private:
        struct renderable*      m_renderable;
};

/** \brief Geometry Renderable.
 */
class GeometryRenderable : public Renderable
{
public:
        GeometryRenderable(struct rda_geometry* geometry);
        ~GeometryRenderable();

        void                    init_from_data ( struct point3d* vertex, int num_vert, int* index, int num_tri,
                                                 struct vector3d* normal, struct vector3d* tangent, struct point2d* uv,
                                                 struct matrix4x4* transform );
        void                    refine ( float iteration );
        void                    update_vertex ( struct point3d* vertex, int count );
        void                    update_index ( int* index, int count );
        void                    set_transform ( struct matrix4x4 *transform );
        void                    auto_generate_normal_and_tangent ();

        struct point3d*         get_vertex ( int *num_vertex );
        struct vector3d*        get_normal ( int* num_normal );
        struct vector3d*        get_tangent ( int* num_tangent );
        struct vector2d*        get_uv ( int* num_uv );
        int*                    get_index ( int* num_index );
private:
        struct rda_geometry*    m_geometry;
};

/** \brief Renderable instance.
 */
class RenderableInstance : public CoreResource
{
public:
        RenderableInstance(struct rda_instance* instance);
        ~RenderableInstance ();

        Renderable* get_source ();
        struct rda_instance* get_core_resource();
private:
        struct rda_instance*     m_instance;
};

}// namespace usr

}// namespace x3d


#endif // USR_RENDERABLE_HPP_INCLUDED
