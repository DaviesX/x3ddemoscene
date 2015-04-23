#include <usr/usr_x3d.hpp>
#include <usr/usr_renderable.hpp>


namespace x3d
{
namespace usr
{

/* Renderable */

/** \brief Construct a renderable.
 *
 * \param renderable struct renderable* core resource.
 *
 */
Renderable::Renderable(struct renderable* renderable)
{
        m_renderable = renderable;
}

/** \brief set the importance of the renderable.
 *
 * This affects the activeness, or visibility, of the renderable.
 *
 * \param importance float importance value between 0 - 10
 * \return void
 *
 */
void Renderable::set_importance(float importance)
{
        rda_set_importance(m_renderable, importance);
}

/** \brief //@fixme (davis#1#): needs to be material  itself, not the reference.
 *
 * \param mater_ref int
 * \return void
 *
 */
void Renderable::set_material(int mater_ref)
{
        rda_set_material(m_renderable, mater_ref);
}

/** \brief get the name of the renderable.
 *
 * \return string the name.
 *
 */
string Renderable::get_name()
{
        return string(rda_get_name(m_renderable));
}

/** \brief get the type of the renderable.
 *
 * \return RenderableFactory::RenderableDesc the type.
 *
 */
RenderableFactory::RenderableDesc Renderable::get_desc()
{
        return (RenderableFactory::RenderableDesc) rda_get_type(m_renderable);
}

/** \brief create instance of the current renderable.
 *
 * \param transform struct matrix4x4* place to instantiate.
 * \return RenderableInstance instant created from the renderable.
 *
 */
RenderableInstance* Renderable::make_instance(struct matrix4x4* transform)
{
        return new RenderableInstance(rda_instance_create(m_renderable, transform));
}

/** \brief get the core resource.
 */
struct renderable* Renderable::get_core_resource()
{
        return m_renderable;
}

void Renderable::set_core_resource(struct renderable* r)
{
        m_renderable = r;
}

/* RenderableFactory */
/** \brief construct a renderable.
 *
 * \param desc RenderableDesc type of renderable needed to create.
 * \param name string name(unique) of the renderable.
 * \param movable bool if the renderable is static, or the otherwise, dynamic
 * \return Renderable* the created renderable.
 *
 */
Renderable* RenderableFactory::create(RenderableDesc desc, string name, bool movable)
{
        switch(desc) {
        case RenderableFactory::Geometry: {
                return new GeometryRenderable(rda_geometry_create(const_cast<char*>(name.c_str()), 0.0f, movable, 0));
        }
        default: {
                log_mild_err_dbg("unknown renderable description: %d", desc);
                return nullptr;
        }
        }
}

/* GeometryRenderable */
/** \brief construct a goemetry renderable.
 *
 * \param geometry struct rda_geometry* core resource.
 *
 */
GeometryRenderable::GeometryRenderable(struct rda_geometry* geometry) :
        Renderable((struct renderable*) geometry)
{
        m_geometry = geometry;
}

/** \brief destructor.
 */
GeometryRenderable::~GeometryRenderable()
{
        rda_free((struct renderable*) m_geometry);
}

/** \brief initialize a geometry renderable from given data.
 *
 * \param vertex struct point3d* vertices of the geometry(array).
 * \param num_vert int the number of vertex.
 * \param index int* face indices(index array of triangles).
 * \param num_tri int the number of triangle face.
 * \param normal struct vector3d* normal of each vertex(array).
 * \param tangent struct vector3d* tangent of each vertex(array).
 * \param uv struct point2d* uv coordinate at each vertex(array).
 * \param transform struct matrix4x4* initial transformation to apply to the the geometry renderable.
 * \return void
 *
 */
void GeometryRenderable::init_from_data(struct point3d* vertex, int num_vert,
                                        int* index, int num_tri, struct vector3d* normal,
                                        struct vector3d* tangent, struct point2d* uv,
                                        struct matrix4x4* transform)
{
        rda_geometry_init_from_data(m_geometry, vertex, num_vert, index, num_tri, normal, tangent, uv, transform);
}

/** \brief refine the geometry.
 *
 * \param iteration float number of refinement steps to be taken.
 * \return void
 *
 */
void GeometryRenderable::refine(float iteration)
{
        rda_geometry_refine(m_geometry, iteration);
}

/** \brief change to vertex of the geometry.
 *
 * \param vertex struct point3d* the vertex array.
 * \param count int number of element in the vertex array.
 * \return void
 *
 */
void GeometryRenderable::update_vertex(struct point3d* vertex, int count)
{
        rda_geometry_update_vertex(m_geometry, vertex, count);
}

/** \brief change the triangle faces of the geometry.
 *
 * \param index int* index array of new triangle faces.
 * \param count int number of triangle vertices(number of elements in the index array).
 * \return void
 *
 */
void GeometryRenderable::update_index(int* index, int count)
{
        rda_geometry_update_index(m_geometry, index, count);
}

/** \brief change the transformation of the geometry.
 *
 * \param transform struct matrix4x4* transformation to be applied.
 * \return void
 *
 */
void GeometryRenderable::set_transform(struct matrix4x4* transform)
{
        rda_geometry_set_transform(m_geometry, transform);
}

/** \brief automatically generate(override the original) normal and tangent for the geometry renderable.
 *
 * \return void
 *
 */
void GeometryRenderable::auto_generate_normal_and_tangent()
{
        rda_geometry_fix_nt(m_geometry);
}

/** \brief get the vertex array of the geoemtry renderable.
 *
 * \param num_normal int* return the number of vertex elements inside the array.
 * \return struct point3d* the vertex array.
 */
struct point3d* GeometryRenderable::get_vertex(int* num_vertex)
{
        return rda_geometry_get_vertex(m_geometry, num_vertex);
}

/** \brief get the normal array of the geometry renderable.
 *
 * \param num_normal int* return the number of normal elements inside the array.
 * \return struct point3d* the normal array.
 */
struct vector3d* GeometryRenderable::get_normal(int* num_normal)
{
        return rda_geometry_get_normal(m_geometry, num_normal);
}

/** \brief get the tangent array of the geometry renderable.
 *
 * \param num_tangent int* return the number of tangent elements inside the array.
 * \return struct vector3d* the tangent array.
 *
 */
struct vector3d* GeometryRenderable::get_tangent(int* num_tangent)
{
        return rda_geometry_get_tangent(m_geometry, num_tangent);
}

/** \brief get the uv array of the geometry renderable.
 *
 * \param num_uv int* return the number of uv elements inside the array.
 * \return struct vector2d* the uv array.
 *
 */
struct vector2d* GeometryRenderable::get_uv(int* num_uv)
{
        return rda_geometry_get_uv(m_geometry, num_uv);
}

/** \brief get the face vertex array of the geometry renderable.
 *
 * \param num_index int* return the number of face vertex elements inside the array.
 * \return int* the face vertex array.
 *
 */
int* GeometryRenderable::get_index(int* num_index)
{
        return rda_geometry_get_index(m_geometry, num_index);
}

/* RenderableInstance */
/** \brief construct an renderable instance.
 *
 * \param instance struct rda_instance* core resource.
 *
 */
RenderableInstance::RenderableInstance(struct rda_instance* instance)
{
        m_instance = instance;
}

/** \brief destructor.
 */
RenderableInstance::~RenderableInstance()
{
        rda_instance_free(m_instance);
}

/** \brief get the renderable from which the instance is created.
 *
 * \return Renderable* the source renderable.
 *
 */
Renderable* RenderableInstance::get_source()
{
        return new Renderable(rda_instance_source(m_instance));
}

/** \brief get core resource.
 */
struct rda_instance* RenderableInstance::get_core_resource()
{
        return m_instance;
}

void RenderableInstance::set_core_resource(struct rda_instance* r)
{
        m_instance = r;
}

}// namespace usr

}// namespace x3d
