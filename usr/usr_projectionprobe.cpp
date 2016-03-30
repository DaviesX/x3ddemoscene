#include <usr/usr_display.hpp>
#include <usr/usr_projectionprobe.hpp>


namespace x3d
{
namespace usr
{

/* ProjectionProbe */
ProjectionProbe::ProjectionProbe(struct projection_probe* probe)
{
        m_probe = probe;
}

ProjectionProbe::~ProjectionProbe()
{
        projprobe_free(m_probe);
}

void ProjectionProbe::set_position(struct point3d* p)
{
        projprobe_set_position(m_probe, p);
}

void ProjectionProbe::set_base(struct vector3d* u, struct vector3d* v, struct vector3d* n)
{
        projprobe_set_base(m_probe, u, v, n);
}

void ProjectionProbe::rotate(float x, float y, float z)
{
        projprobe_rotate(m_probe, x, y, z);
}

void ProjectionProbe::rotate_relative(float x, float y, float z)
{
        projprobe_rotate_relative(m_probe, x, y, z);
}

void ProjectionProbe::move(float x)
{
        projprobe_move(m_probe, x);
}

void ProjectionProbe::move_relative(float x)
{
        projprobe_move_relative(m_probe, x);
}

void ProjectionProbe::transform_base(struct matrix4x4* transform)
{
        projprobe_transform_base(m_probe, transform);
}

void ProjectionProbe::transform_base_relative(struct matrix4x4* transform)
{
        projprobe_transform_base_relative(m_probe, transform);
}

struct projection_probe* ProjectionProbe::get_core_resource()
{
        return m_probe;
}

/* PerspectiveProbe */
PerspectiveProbe::PerspectiveProbe(Display* display, int xres, int yres) :
        ProjectionProbe(&persprobe_create(display->get_core_resource(), xres, yres)->_parent)
{
        m_probe = (struct perspective_probe*) (static_cast<ProjectionProbe*> (this))->m_probe;
        persprobe_set_range(m_probe, xres, yres, 2000.0f);
        persprobe_set_optics(m_probe, 35.0f, 200.0f, 35.0f/8.0f);
}

PerspectiveProbe::~PerspectiveProbe()
{
        m_probe = nullptr;
}

void PerspectiveProbe::set_optics(float focal_length, float v_dist, float aperture)
{
        persprobe_set_optics(m_probe, focal_length, v_dist, aperture);
}

void PerspectiveProbe::set_direction(struct vector3d* up, struct point3d* target)
{
        persprobe_set_direction(m_probe, up, target);
}

void PerspectiveProbe::set_range(float x, float y, float z)
{
        persprobe_set_range(m_probe, x, y, z);
}

/* OrthogonalProbe*/
OrthogonalProbe::OrthogonalProbe(Display* display, int xres, int yres) :
        ProjectionProbe(&orthoprobe_create(display->get_core_resource(), xres, yres)->_parent)
{
}

OrthogonalProbe::~OrthogonalProbe()
{
}

}// namespace usr

}// namespace x3d
